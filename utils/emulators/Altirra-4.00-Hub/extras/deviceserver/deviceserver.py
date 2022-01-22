# Device server for Altirra custom devices (V2 / Altirra 4.0 protocol)
# Copyright (C) 2020 Avery Lee, All rights reserved.
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any
# damages arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 
# 1. The origin of this software must not be misrepresented; you must
#    not claim that you wrote the original software. If you use this
#    software in a product, an acknowledgment in the product
#    documentation would be appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must
#    not be misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source
#    distribution.


import socketserver
import struct
import signal
import sys
import argparse

class DeviceSegment:
    """
    Proxy for Segment variables in the device script.
    """

    __slots__ = ['handler', 'segment_index']

    def __init__(self, handler, segment_index):
        self.handler = handler
        self.segment_index = segment_index

    def read(self, offset: int, len: int):
        """
        Read data from [offset:offset+len] in the segment.
        """
        return self.handler.req_read_seg_mem(self.segment_index, offset, len)

    def write(self, offset: int, data:bytes):
        """
        Write data to [offset:offset+len] in the segment.
        """
        return self.handler.req_write_seg_mem(self.segment_index, offset, data)

    def fill(self, offset: int, val: int, len: int):
        """
        Fill [offset:offset+len] with byte val.
        """

        return self.handler.req_fill_seg_mem(self.segment_index, offset, val, len)

    def copy(self, dst_offset: int, src_segment: 'DeviceSegment', src_offset: int, len: int):
        """
        Copy src_segment[src_offset:src_offset+len] to dst_offset[dst_offset:dst_offset+len].
        """

        return self.handler.req_copy_seg_mem(self.segment_index, dst_offset, src_segment.segment_index, src_offset, len)

class DeviceMemoryLayer:
    """
    Proxy for MemoryLayer variables in the device script.
    """

    __slots__ = ['handler', 'layer_index']

    def __init__(self, handler, layer_index):
        self.handler = handler
        self.layer_index = layer_index

    def enable(self, read: bool, write: bool):
        """
        Enable or disable the memory layer for read and write memory accesses.
        """

        self.handler.req_enable_layer(self.layer_index, read, write)

    def set_offset(self, offset: int):
        """
        Set the starting byte offset of a direct mapped layer within the mapped segment.
        """

        self.handler.req_set_layer_offset(self.layer_index, offset)

    def set_segment_and_offset(self, segment: DeviceSegment, segment_offset: int):
        """
        Set the mapped segment and starting byte offset for a direct mapped layer.
        """

        self.handler.req_set_layer_segment_and_offset(self.layer_index, segment.segment_index, segment_offset)

    def set_readonly(self, ro: bool):
        """
        Set the read-only status of a layer, controlling whether writes to the layer
        change the data in the mapped segment.
        """

        self.handler.req_set_layer_readonly(self.layer_index, ro)

class DeviceTCPHandler(socketserver.BaseRequestHandler):
    """
    Base socketserver handler for implementing the custom device server
    protocol. You should subclass this type in your own code and implement
    handle_*() methods. Use req_*() methods or the methods on the reflected
    device objects to call back into the emulator.

    For convenience, memory layers and segment variables are reflected from the
    device script back into this handler to allow them to be referred to by name,
    isolating the server from some changes in the device script. A segment variable
    'foo' becomes self.seg_foo, and a memory layer 'bar' becomes self.layer_bar.
    """

    def __init__(self, *args, **kwargs):
        self.verbose = False
        self.handlers = {};

        self.handlers[0] = ("None", self.handle_none)
        self.handlers[1] = ("Debug read byte", self.wrap_debugreadbyte)
        self.handlers[2] = ("Read byte", self.wrap_readbyte)
        self.handlers[3] = ("Write byte", self.wrap_writebyte)
        self.handlers[4] = ("Cold reset", self.wrap_coldreset)
        self.handlers[5] = ("Warm reset", self.wrap_warmreset)
        self.handlers[6] = ("Error", self.handle_error)
        self.handlers[7] = ("Script event", self.wrap_script_event)
        self.handlers[8] = ("Script post", self.handle_script_post)

        self.counter = 0

        super().__init__(*args, **kwargs)

    def handle(self):
        """
        Main protocol service loop.
        """

        self.verbose = self.server.cmdline_args.verbose

        print("Connection received from emulator")

        while True:
            command_packet = bytearray()

            while len(command_packet) < 17:
                command_subpacket = self.request.recv(17 - len(command_packet))
                if len(command_subpacket) == 0:
                    print("Connection closed")
                    return

                command_packet.extend(command_subpacket)

            command_id, param1, param2, timestamp = struct.unpack('<BIiQ', command_packet)

            try:
                command_name, handler = self.handlers[command_id]
            except KeyError:
                print("Unhandled command {:02X} - closing connection.".format(command_id))
                return

            if self.verbose:
                print("{1:016X} {0}({2:08X}, {3:08X})".format(command_name, timestamp, param1, param2))

            handler(param1, param2, timestamp)

    #----------------------------------------------------------------------------------
    # Raw extension points
    #
    # These adapt the raw protocol interface calls to more appropriate arguments and
    # return values. Typically these are not used unless there is some need to
    # intercept or instrument the raw protocol.
    #
    def wrap_debugreadbyte(self, address, param2, timestamp) -> int:
        rvalue = self.handle_debugreadbyte(address, timestamp)

        self.request.sendall(struct.pack('<Bi', 1, rvalue))

    def wrap_readbyte(self, address, param2, timestamp) -> int:
        rvalue = self.handle_readbyte(address, timestamp)

        self.request.sendall(struct.pack('<Bi', 1, rvalue))

    def wrap_writebyte(self, param1, param2, timestamp) -> int:
        self.handle_writebyte(param1, param2, timestamp)
        self.request.sendall(b'\x01\0\0\0\0')

    def wrap_coldreset(self, param1, param2, timestamp) -> int:
        # The V1 protocol unfortunately lacks an extension point and rejects unknown
        # calls, so the host abuses the cold reset command with different parameters as
        # the init command to detect downlevel hosts. We do not support V1 hosts here
        # but must still play along.
        if param2 >= 0x7F000001 and param2 <= 0x7FFFFFFF:
            # tell host that we support V2 protocol
            self.request.sendall(b'\x0C\x02')
            self.wrap_init()

        self.handle_coldreset(timestamp)
        self.request.sendall(b'\x01\0\0\0\0')

    def wrap_warmreset(self, param1, param2, timestamp) -> int:
        self.handle_warmreset(timestamp)
        self.request.sendall(b'\x01\0\0\0\0')

    def wrap_script_event(self, param1, param2, timestamp) -> int:
        self.request.sendall(struct.pack('<Bi', 1, self.handle_script_event(param1, param2, timestamp)))

    def wrap_init(self):
        self.reflect_vars()

    #----------------------------------------------------------------------------------
    # Extension points
    #
    # These are expected to be selectively replaced by subclasses to implement server
    # behavior. There is generally no need to chain back to super.
    #

    def handle_none(self, param1, param2, timestamp) -> int:
        pass

    def handle_debugreadbyte(self, address, timestamp) -> int:
        """
        Handler invoked on a debug read from a network-based memory layer binding.
        The return value is the byte that would be read from the given address.
        This must be implemented without side effects; it is expected that debug
        reads may be issued at any time without affecting emulation state.

        The default handler returns an internal counter for initial debugging.
        """

        return self.counter

    def handle_readbyte(self, address, timestamp) -> int:
        """
        Handler invoked on a non-debug read from a network-based memory layer binding.
        The return value is the byte read from the given address. This handler is
        expected to apply any side effects from the read operation, such as clearing
        pending interrupts or advancing an internal read address.

        The default handler returns an internal auto-incrementing counter for initial
        debugging.
        """

        v = self.counter
        self.counter = (self.counter + 1) & 0xFF
        return v

    def handle_writebyte(self, address, value, timestamp) -> None:
        """
        Handler invoked on a write to a network-based memory layer binding.
        """

        self.counter = value

    def handle_init(self) -> None:
        """
        Handler invoked when the emulator is initially connected. Good for once init
        actions that must NOT occur on a cold reset.
        """

        pass

    def handle_coldreset(self, timestamp) -> None:
        """
        Handler invoked when a cold reset (power off/on) occurs in the emulator.
        """

        pass

    def handle_warmreset(self, timestamp) -> None:
        """
        Handler invoked when a warm reset (system reset) occurs in the emulator.
        """

        pass

    def handle_error(self, param1, param2, timestamp) -> int:
        """
        Handler invoked when the emulator detects a device server protocol error.
        This indicates either a programming error in the device server or incompatible
        emulator/server versions.
        """

        msg = self._readall(param2).decode('utf-8')
        print("Error from emulator: " + msg)
        return 0

    def handle_script_event(self, param1, param2, timestamp) -> int:
        """
        Synchronous call from device script to the device server. Emulation is halted
        while this executes and the return value is sent back to the script.
        """

        return 0

    def handle_script_post(self, param1, param2, timestamp) -> None:
        """
        Asynchornous call from device script to the device server. Emulation is NOT
        halted and continues while this is processed by the server.
        """

        pass

    #----------------------------------------------------------------------------------
    # Request functions
    #

    def req_enable_layer(self, layer_index: int, read: bool, write: bool):
        """
        Enable or disable a memory layer for read or write access. A memory layer
        enabled for an access type handles accesses of that type over the covered
        address space and prevents the access from being seen by lower priority layers.
        """

        self.request.sendall(struct.pack('<BBB', 2, layer_index, (2 if read else 0) + (1 if write else 0)))

    def req_set_layer_offset(self, layer_index: int, offset: int):
        """
        Change the starting byte offset for a layer. The starting offset must be
        page-aligned and the resulting mapped window must be entirely within the
        segment. It is a fatal error otherwise, though not all cases are diagnosed
        here.
        """

        if offset < 0 or offset & 255:
            raise ValueError('Invalid segment offset')

        self.request.sendall(struct.pack('<BBI', 3, layer_index, offset))

    def req_set_layer_segment_and_offset(self, layer_index: int, segment_index: int, segment_offset: int):
        """
        Change both the mapped segment and starting byte offset for a layer. The
        segment must be mappable, the offset must be page aligned, and the resulting
        mapped region must be completely contained within the segment.
        """

        if segment_offset < 0 or segment_offset & 255:
            raise ValueError('Invalid segment offset')

        self.request.sendall(struct.pack('<BBBI', 4, layer_index, segment_index, segment_offset))

    def req_set_layer_readonly(self, layer_index: int, ro: bool):
        """
        Change the read-only status of a layer. If a layer is read-only, writes to
        the segment are blocked from lower priority segments but the segment
        is not changed. This can only be applied to direct-mapped layers and not
        control layers.
        """

        self.request.sendall(struct.pack('<BBB', 5, layer_index, 1 if ro else 0))

    def req_read_seg_mem(self, segment_index: int, offset: int, len: int):
        """
        Read a block of memory from a segment as a bytes object. The region to read
        must be fully contained within the segment.
        """

        if offset < 0:
            raise ValueError('Invalid segment offset')

        if len <= 0:
            if len == 0:
                return bytes()

            raise ValueError('Invalid length')


        self.request.sendall(struct.pack('<BBII', 6, segment_index, offset, len))
        return self._readall(len)

    def req_write_seg_mem(self, segment_index: int, offset: int, data:bytes):
        """
        Write data from a bytes object to a segment. The region to write must be
        fully contained within the segment.
        """

        if offset < 0:
            raise ValueError('Invalid segment offset')

        if len(data) == 0:
            return

        self.request.sendall(struct.pack('<BBII', 7, segment_index, offset, len(data)))
        self.request.sendall(data)

    def req_fill_seg_mem(self, segment_index: int, offset: int, val: int, len: int):
        """
        Fill a region of a segment with a constant value. The region to fill must be
        fully contained within the segment.
        """

        if offset < 0:
            raise ValueError('Invalid segment offset')

        if len <= 0:
            if len == 0:
                return

            raise ValueError('Invalid fill length')

        self.request.sendall(struct.pack('<BBIBI', 13, segment_index, offset, val, len))

    def req_copy_seg_mem(self, dst_segment_index: int, dst_offset: int, src_segment_index: int, src_offset: int, len: int):
        """
        Copy data from one segment region to another. The source and destination
        regions must be entirely contained within their respective segments. The
        source and destination regions may be within the same segment and may overlap;
        if so, the copy direction is selected so that the entire contents of the source
        region are preserved in the copy to the destination (memmove semantics).
        """

        if dst_offset < 0:
            raise ValueError('Invalid destination segment offset')

        if src_offset < 0:
            raise ValueError('Invalid source segment offset')

        if len < 0:
            raise ValueError('Invalid copy length')

        if len > 0:
            self.request.sendall(struct.pack('<BBIBII', 8, dst_segment_index, dst_offset, src_segment_index, src_offset, len))

    def req_interrupt(self, aux1: int, aux2: int):
        """
        Issue a script interrupt with the arguments (aux1, aux2). This requires an
        accompanying binding in the device script to the script interrupt event, which
        is invoked in response.
        """

        self.request.sendall(struct.pack('<BII', 9, aux1, aux2))

    def req_get_segment_names(self):
        """
        Retrieve a list of segment variable names from the device script, in ascending
        index order (first = index 0). This is not normally called directly as the init
        handler will call it and reflect the segment names.
        """

        self.request.sendall(b'\x0A')
        return self._read_names()

    def req_get_layer_names(self):
        """
        Retrieve a list of memory layer names from the device script, in ascending
        index order (first = index 0). This is not normally called directly as the init
        handler will call it and reflect the memory layer names.
        """

        self.request.sendall(b'\x0B')
        return self._read_names()

    def reflect_vars(self):
        """
        Reflect variables from the device script into the handler so that segments and
        memory layers can be referred to by name. Segments are prefixed with seg_ and
        memory layers with layer_.
        """

        for segment_index, segment_name in enumerate(self.req_get_segment_names(), start=0):
            setattr(self, "seg_" + segment_name, DeviceSegment(self, segment_index))

        for layer_index, layer_name in enumerate(self.req_get_layer_names(), start=0):
            setattr(self, "layer_" + layer_name, DeviceMemoryLayer(self, layer_index))

    def _read_names(self):
        """
        Read a list of names from the inbound stream in the form: name count,
        [name len, name chars*]*.
        """

        num_names = struct.unpack('<I', self._readall(4))[0]
        names = []

        for i in range(0, num_names):
            name_len = struct.unpack('<I', self._readall(4))[0]
            print(name_len)
            names.append(self._readall(name_len).decode('utf-8'))

        return names

    def _readall(self, readlen):
        """
        Read the specified number of bytes from the inbound stream, blocking if
        necessary until they all arrive.
        """

        seg_data = bytearray()
        while len(seg_data) < readlen:
            seg_subdata = self.request.recv(readlen - len(seg_data))
            if len(seg_subdata) == 0:
                raise ConnectionError

            seg_data.extend(seg_subdata)

        return seg_data

def print_banner():
    print("Altirra Custom Device Server v0.8")
    print()

def run_deviceserver(
    handler: type,
    port: int = 6502,
    arg_parser = argparse.ArgumentParser(description = "Starts a localhost TCP server to handle emulator requests for a custom device."),
    run_handler = None,
    post_argparse_handler = None
):
    """
    Bootstrap the device server. Call this from your startup module to print
    the startup banner, parse command line arguments, and run the TCP server.

    To add arguments, override the default argument parser and supply a pre-populated
    instance via the arg_parser argument. The parsed arugments are passed to
    post_argparse_handler() and stashed as self.server.cmdline_args.
    """

    print_banner()

    arg_parser.add_argument('--port', type=int, default=port, help='Change TCP port (default: {})'.format(port))
    arg_parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Log emulation device commands')

    args = arg_parser.parse_args()

    if post_argparse_handler is not None:
        post_argparse_handler(args)

    with socketserver.TCPServer(("localhost", args.port), handler) as server:
        server.cmdline_args = args

        print("Waiting for localhost connection from emulator on port {} -- Ctrl+Break to stop".format(args.port))

        if run_handler is not None:
            run_handler(server)
        else:
            server.serve_forever()

if __name__ == '__main__':
    print_banner()
    print("""deviceserver.py is not meant to be run directly. It is a framework for
building your own device server to be used with a custom device specified in an
.atdevice file.""")
