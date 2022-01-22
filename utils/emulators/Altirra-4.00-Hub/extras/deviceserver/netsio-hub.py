#!/usr/bin/env python3

from enum import IntEnum
import deviceserver
import socket, socketserver
import threading
import queue
import sys
import time
import struct
import argparse
from timeit import default_timer as timer
from datetime import datetime

HUB_VERSION = "v0.1"

NETSIO_DATA_BYTE        = 0x01
NETSIO_DATA_BLOCK       = 0x02
NETSIO_DATA_BYTE_SYNC   = 0x09
NETSIO_COMMAND_OFF      = 0x10
NETSIO_COMMAND_ON       = 0x11
NETSIO_COMMAND_OFF_SYNC = 0x18
NETSIO_MOTOR_OFF        = 0x20
NETSIO_MOTOR_ON         = 0x21
NETSIO_PROCEED_OFF      = 0x30
NETSIO_PROCEED_ON       = 0x31
NETSIO_INTERRUPT_OFF    = 0x40
NETSIO_INTERRUPT_ON     = 0x41
NETSIO_SPEED_CHANGE     = 0x80
NETSIO_SYNC_RESPONSE    = 0x81
NETSIO_DEVICE_DISCONNECT = 0xC0
NETSIO_DEVICE_CONNECT   = 0xC1
NETSIO_PING_REQUEST     = 0xC2
NETSIO_PING_RESPONSE    = 0xC3
NETSIO_ALIVE_REQUEST    = 0xC4
NETSIO_ALIVE_RESPONSE   = 0xC5
NETSIO_WARM_RESET       = 0xFE
NETSIO_COLD_RESET       = 0xFF

# events to manage device connection (connect, ping, alive) >= 0xC0
NETSIO_CONN_MGMT        = 0xC0

# NETSIO_SYNC_RESPONSE types
NETSIO_EMPTY_SYNC       = 0x00
NETSIO_ACK_SYNC         = 0x01

# Altirra specific
ATDEV_READY             = 0x100
ATDEV_TRANSMIT_BUFFER   = 0x101
ATDEV_DEBUG_MESSAGE     = 0x102
ATDEV_EMPTY_SYNC        = -1 # any value less than zero


# local TCP port for Altirra custom device communication
NETSIO_ATDEV_PORT   = 9996
# UDP port NetSIO is accepting messages from peripherals
NETSIO_PORT         = 9997

# client expiration period in seconds
#  if NetSIO HUB will not receive alive message (NETSIO_ALIVE_REQUEST) from device the device
#  connection is being considered as expired and the device is disconnected from the HUB
ALIVE_EXPIRATION = 30.0


# debug printing, disabled by default
debug_print = lambda *a, **k: None

_start_time = timer()

def print_banner():
    print("NetSIO HUB", HUB_VERSION)


def clear_queue(q):
    try:
        while True:
            q.get_nowait()
    except queue.Empty:
        pass


class NetSIOMsg:
    def __init__(self, id, arg=None):
        self.id = id
        if arg is None:
            self.arg = bytes()
        else:
            self.arg = arg if isinstance(arg, (bytes, bytearray)) else struct.pack('B', arg)
        self.time = timer()

    def elapsed(self):
        return timer() - self.time

    def arg_str(self):
        return " ".join(["{:02X}".format(b) for b in self.arg])

    def __str__(self):
        return "{:02X}{}{}".format(
            self.id, " " if len(self.arg) else"", self.arg_str())


class NetSIOClient:
    def __init__(self, address, sock):
        self.address = address
        self.sock = sock
        self.expire_time = time.time() + ALIVE_EXPIRATION
        self.cpb = 93; # defalt 93 CPB (19200 baud)


class NetInThread(threading.Thread):
    """Thread to handle incoming network traffic"""
    def __init__(self, hub, port):
        self.hub = hub
        self.port = int(port)
        self.server = None
        self.server_ready = threading.Event()
        super().__init__()

    def run(self):
        debug_print("NetInThread started")
        with NetSIOServer(self.hub, self.port) as self.server:
            print("Listening for NetSIO packets on port {}".format(self.port))
            self.server_ready.set()
            self.server.serve_forever()
        self.server_ready.clear()
        debug_print("NetInThread stopped")

    def stop(self):
        debug_print("Stop NetInThread")
        self.server.shutdown()


class NetSIOServer(socketserver.UDPServer):
    """NetSIO UDP Server"""
    def __init__(self, hub, port):
        self.hub = hub
        self.clients_lock = threading.Lock()
        self.clients = {}
        self.last_recv = timer()
        self.sn = 0 # TODO test only
        super().__init__(('', port), NetSIOHandler)

    def register_client(self, address, sock):
        with self.clients_lock:
            if address not in self.clients:
                client = NetSIOClient(address, sock)
                self.clients[address] = client
                info_print("Device connected {}:{} [{}]".format(address[0], address[1], len(self.clients)))
            else:
                client = self.clients[address]
                client.sock = sock
                client.expire_time = ALIVE_EXPIRATION + time.time()
                info_print("Device reconnected {}:{} [{}]".format(address[0], address[1], len(self.clients)))
            self.hub.handle_device_msg(NetSIOMsg(NETSIO_DEVICE_CONNECT), client)
        return client

    def deregister_client(self, address, expired=False):
        with self.clients_lock:
            try:
                client = self.clients.pop(address)
            except KeyError:
                client = None
            count = len(self.clients)
        if client is not None:
            info_print("Device disconnected {}{}:{} [{}]".format(
                "(connection expired) " if expired else "", address[0], address[1], count))
            self.hub.handle_device_msg(NetSIOMsg(NETSIO_DEVICE_DISCONNECT), client)

    def get_client(self, address):
        with self.clients_lock:
            client = self.clients.get(address)
        return client

    def send_to_clients(self, msg):
        """broadcast all connected netsio devices"""
        t = time.time()
        with self.clients_lock:
            # clients = self.clients.values()
            clients = [c for c in self.clients.values() if c.expire_time > t]
        if clients:
            for c in clients:
                # c.sock.sendto(struct.pack('B', msg.id) + msg.arg, c.address)
                # TODO test only
                c.sock.sendto(struct.pack('B', msg.id) + msg.arg + struct.pack('B', self.sn), c.address)
            debug_print("> NET +{:.0f} {} [{}] {} {:02X}".format(
                        msg.elapsed() * 1.e6,
                        " ".join(["{}:{}".format(c.address[0], c.address[1]) for c in clients]),
                        1+len(msg.arg), 
                        msg,
                        self.sn))
            self.sn = (self.sn +1) & 255
            # remove expired clients
            self.expire_clients()
    
    def expire_clients(self):
        t = time.time()
        with self.clients_lock:
            expired = [c for c in self.clients.values() if c.expire_time <= t]
        for c in expired:
            self.deregister_client(c.address, expired=True)
        
    def connected(self):
        """Return true if any client is connected"""
        with self.clients_lock:
            return len(self.clients) > 0

class NetSIOHandler(socketserver.BaseRequestHandler):
    """NetSIO received packet handler"""

    def handle(self):
        data, sock = self.request
        msg = NetSIOMsg(data[0], data[1:])
        ca = self.client_address

        debug_print("< NET +{:.0f} {}:{} [{}]".format(
            (timer()-self.server.last_recv)*1.e6,
            ca[0], ca[1], 1+len(msg.arg)), msg)
        self.server.last_recv = timer()

        if msg.id < NETSIO_CONN_MGMT:
            # events from connected/registered devices
            client = self.server.get_client(self.client_address)
            if client is not None:
                if client.expire_time > time.time():
                    self.server.hub.handle_device_msg(msg, client)
                else:
                    self.server.deregister_client(client.address, expired=True)
        else:
            # connection management
            if msg.id == NETSIO_DEVICE_DISCONNECT:
                # device disconnected, deregister client
                self.server.deregister_client(self.client_address)
            elif msg.id == NETSIO_DEVICE_CONNECT:
                # remove expired clients, if any
                self.server.expire_clients()
                # device connected, register client for netsio messages
                self.server.register_client(self.client_address, sock)
            elif msg.id == NETSIO_PING_REQUEST:
                # ping request, send ping response (always)
                # TODO send_to_client
                sock.sendto(struct.pack('B', NETSIO_PING_RESPONSE), self.client_address)
                debug_print("> NET {}:{} [1] {:02X}".format(
                    self.client_address[0], self.client_address[1], NETSIO_PING_RESPONSE))
            elif msg.id == NETSIO_ALIVE_REQUEST:
                # alive, send alive response (only if connected/registered)
                client = self.server.get_client(self.client_address)
                if client is not None:
                    # TODO send_to_client
                    client.expire_time = ALIVE_EXPIRATION + time.time()
                    sock.sendto(struct.pack('B', NETSIO_ALIVE_RESPONSE), self.client_address)
                    debug_print("> NET {}:{} [1] {:02X}".format(
                        self.client_address[0], self.client_address[1], NETSIO_ALIVE_RESPONSE))


class NetOutThread(threading.Thread):
    """Thread to send "messages" to connected netsio devices"""
    def __init__(self, q, server):
        self.queue = q
        self.server = server
        super().__init__()

    def run(self):
        debug_print("NetOutThread started")
        while True:
            msg = self.queue.get()
            if msg is None:
                break
            self.server.send_to_clients(msg)

        debug_print("NetOutThread stopped")

    def stop(self):
        debug_print("Stop NetOutThread")
        clear_queue(self.queue)
        self.queue.put(None) # stop sign
        self.join()


class NetSIOManager():
    """Manages NetSIO (SIO over UDP) traffic"""

    def __init__(self, port=NETSIO_PORT):
        self.port = port
        self.device_queue = queue.Queue(16)
        self.netin_thread = None
        self.netout_thread = None

    def start(self, hub):
        # create and start threads

        # network receiver
        self.netin_thread = NetInThread(hub, self.port)
        self.netin_thread.start()

        # wait for server to be created
        if not self.netin_thread.server_ready.wait(3):
            print("Time out waiting for NetSIOServer to start")

        # network sender
        self.netout_thread = NetOutThread(self.device_queue, self.netin_thread.server)
        self.netout_thread.start()

    def stop(self):
        if self.netin_thread:
            self.netin_thread.stop()
            self.netin_thread = None
        if self.netout_thread:
            self.netout_thread.stop()
            self.netout_thread = None

    def to_peripheral(self, msg):
        if msg.id in (NETSIO_COLD_RESET, NETSIO_WARM_RESET):
            debug_print("CLEAR DEV QUEUE")
            clear_queue(self.device_queue)

        if self.device_queue.full():
            debug_print("device queue FULL")
        else:
            debug_print("device queue [{}]".format(self.device_queue.qsize()))

        self.device_queue.put(msg)
        # debug_print("> DEV", msg)

    def connected(self):
        """Return true if any device is connected"""
        return self.netin_thread.server.connected()

class HostManager():
    """Connection with Atari host, i.e. connection with Atari emulator"""
    def __init__(self):
        self.hub = None
        pass

    def run(self, hub):
        self.hub = hub
        pass

    def stop(self):
        pass

class AtDevManager(HostManager):
    """Altirra custom device manager"""
    def __init__(self, arg_parser):
        super().__init__()
        self.arg_parser = arg_parser

    def run(self, hub):
        self.hub = hub
        deviceserver.run_deviceserver(AtDevHandler, NETSIO_ATDEV_PORT, self.arg_parser, self.run_server)

    def run_server(self, server):
        # make hub available to handler (via server object)
        server.hub = self.hub
        server.serve_forever()

    def stop(self):
        # TODO stop AtDevThread, if still running
        pass

class AtDevHandler(deviceserver.DeviceTCPHandler):
    """Handler to communicate with netsio.atdevice which lives in Altirra"""
    def __init__(self, *args, **kwargs):
        debug_print("AtDevHandler")
        self.hub = None
        self.atdev_ready = None
        self.atdev_thread = None
        self.busy_at = timer()
        super().__init__(*args, **kwargs)

    def handle(self):
        """handle messages from netsio.atdevice"""
        # start thread for outgoing messages to atdevice
        self.hub = self.server.hub
        self.atdev_ready = threading.Event()
        self.atdev_ready.set()
        host_queue = self.hub.host_connected(self)
        self.atdev_thread = AtDevThread(host_queue, self)
        self.atdev_thread.start()

        try:
            super().handle()
        finally:
            self.hub.host_disconnected()
            self.atdev_thread.stop()

    def handle_script_post(self, event: int, arg: int, timestamp: int):
        """handle post_message from netsio.atdevice"""
        debug_print("> ATD {:02X} {:02X}".format(event, arg))

        if event == ATDEV_READY:
            # POKEY is ready to receive serial data
            self.set_rtr()
        elif event == NETSIO_DATA_BYTE:
            # serial byte from POKEY
            self.hub.handle_host_msg(NetSIOMsg(event, arg))
        elif event == NETSIO_SPEED_CHANGE:
            # serial output speed changed
            self.hub.handle_host_msg(NetSIOMsg(event, struct.pack("<L", arg)))
        elif event < 0x100: # fit byte
            # all other (one byte) events from atdevice
            if event == NETSIO_COLD_RESET:
                self.atdev_ready.set()
            # send to connected devices
            self.hub.handle_host_msg(NetSIOMsg(event))

    def handle_script_event(self, event: int, arg: int, timestamp: int) -> int:
        debug_print("> SYNC ATD {:02X} {:02X}".format(event, arg))
        result = ATDEV_EMPTY_SYNC
        if event == NETSIO_DATA_BYTE_SYNC:
            msg = NetSIOMsg(event, arg) # request sn will be appended
        elif event == NETSIO_COMMAND_OFF_SYNC:
            msg = NetSIOMsg(event) # request sn will be appended
        elif event == NETSIO_DATA_BLOCK:
            msg = NetSIOMsg(event, self.req_read_seg_mem(1, 0, arg)) # get data from rxbuffer segment
        else:
            info_print("Invalid SYNC ATD request")
            return result
        result = self.hub.handle_host_msg_sync(msg)
        return result

    def handle_coldreset(self, timestamp):
        debug_print("> ATD COLD RESET")
        # In some cases Altirra does send Cold reset message without cold-resetting emulated Atari
        # self.hub.handle_host_msg(NetSIOMsg(NETSIO_COLD_RESET))

    def handle_warmreset(self, timestamp):
        debug_print("> ATD WARM RESET")
        self.hub.handle_host_msg(NetSIOMsg(NETSIO_WARM_RESET))

    def clear_rtr(self):
        debug_print("ATD BUSY")
        self.busy_at = timer()
        self.atdev_ready.clear()

    def set_rtr(self):
        self.atdev_ready.set()
        debug_print("ATD READY +{:.0f}".format((timer()-self.busy_at)*1.e6))

    def wait_rtr(self, timeout):
        return self.atdev_ready.wait(timeout)

class AtDevThread(threading.Thread):
    """Thread to send "messages" to Altrira atdevice"""
    def __init__(self, queue, handler):
        self.queue = queue
        self.atdev_handler = handler
        self.busy_at = timer()
        self.request_to_stop = threading.Event()
        super().__init__()

    def run(self):
        debug_print("AtDevThread started")

        # TODO debug text message
        # place message to netsio.atdevice textbuffer i.e. segment 2
        msg = NetSIOMsg(ATDEV_DEBUG_MESSAGE, b"Hi")
        self.atdev_handler.req_write_seg_mem(2, 0, msg.arg)
        msglen = len(msg.arg)
        debug_print("< ATD +{:.0f} MSG [{}] {}".format(msg.elapsed() * 1.e6, msglen, msg.arg_str()))
        # instruct netsio.atdevice to send rxbuffer to emulated Atari
        self.atdev_handler.req_interrupt(ATDEV_DEBUG_MESSAGE, msglen)
        debug_print("< ATD +{:.0f} {:02X} {:02X}".format(msg.elapsed() * 1.e6, ATDEV_DEBUG_MESSAGE, msglen))

        while True:
            msg = self.queue.get()
            if self.request_to_stop.is_set():
                break

            if not self.atdev_handler.wait_rtr(5): # TODO adjustable
                info_print("ATD TIMEOUT")
                # TODO timeout recovery
                clear_queue(self.queue)
                self.atdev_handler.set_rtr()

            if self.request_to_stop.is_set():
                break

            if msg.id in (NETSIO_DATA_BYTE, NETSIO_DATA_BLOCK):
                # send byte and send buffer makes POKEY busy and
                # we have to receive confirmation when it is ready again
                # prior sending more data
                self.atdev_handler.clear_rtr()

            if msg.id == NETSIO_DATA_BLOCK:
                # place serial data to netsio.atdevice rxbuffer i.e. segment 0
                self.atdev_handler.req_write_seg_mem(0, 0, msg.arg)
                rxsize = len(msg.arg)
                debug_print("< ATD +{:.0f} BUFFER [{}] {}".format(msg.elapsed() * 1.e6, rxsize, msg.arg_str()))
                # instruct netsio.atdevice to send rxbuffer to emulated Atari
                self.atdev_handler.req_interrupt(ATDEV_TRANSMIT_BUFFER, rxsize)
                debug_print("< ATD +{:.0f} {:02X} {:02X}".format(msg.elapsed() * 1.e6, ATDEV_TRANSMIT_BUFFER, rxsize))
            elif msg.id == NETSIO_DATA_BYTE:
                # serial byte from remote device
                self.atdev_handler.req_interrupt(msg.id, msg.arg[0])
                debug_print("< ATD +{:.0f} {}".format(msg.elapsed() * 1.e6, msg))
            elif msg.id == NETSIO_SPEED_CHANGE:
                # speed change
                if len(msg.arg) == 4:
                    self.atdev_handler.req_interrupt(msg.id, struct.unpack('<L', msg.arg)[0])
                    debug_print("< ATD +{:.0f} {}".format(msg.elapsed() * 1.e6, msg))
                else:
                    info_print("Invalid NETSIO_SPEED_CHANGE message")
            else:
                # all other
                self.atdev_handler.req_interrupt(msg.id, msg.arg[0] if len(msg.arg) else 0)
                debug_print("< ATD +{:.0f} {}".format(msg.elapsed() * 1.e6, msg))

        debug_print("AtDevThread stopped")

    def stop(self):
        debug_print("Stop AtDevThread")
        self.request_to_stop.set()
        # clear_queue(self.queue) # things can cumulate here ...
        self.queue.put(None) # unblock queue.get()
        self.join()


class NetSIOHub:
    """HUB connecting NetSIO devices with Atari host"""

    class SyncRequest:
        """Synchronized request-response"""
        def __init__(self):
            self.sn = 0
            self.request = None
            self.response = None
            self.lock = threading.Lock()
            self.completed = threading.Event()

        def set_request(self, request):
            with self.lock:
                self.sn = (self.sn + 1) & 255
                self.request = request
                self.completed.clear()
            return self.sn

        def set_response(self, response, sn):
            with self.lock:
                if self.request is not None and self.sn == sn:
                    self.request = None
                    self.response = response
                    self.completed.set()

        def get_response(self, timeout=None, timout_value=None):
            if self.completed.wait(timeout):
                with self.lock:
                    self.request = None
                    return self.response
            else:
                with self.lock:
                    self.request = None
                    return timout_value

        def check_request(self):
            with self.lock:
                return self.request, self.sn

    def __init__(self, device_manager, host_manager):
        self.device_manager = device_manager
        self.host_manager = host_manager
        self.host_queue = queue.Queue(3)
        self.host_ready = threading.Event()
        self.host_handler = None
        self.sync = NetSIOHub.SyncRequest()

    def run(self):
        try:
            self.device_manager.start(self)
            self.host_manager.run(self)
        finally:
            self.device_manager.stop()
            self.host_manager.stop()

    def host_connected(self, host_handler):
        info_print("Host connected")
        self.host_handler = host_handler
        self.host_ready.set()
        return self.host_queue

    def host_disconnected(self):
        info_print("Host disconnected")
        self.host_ready.clear()
        self.host_handler = None
        clear_queue(self.host_queue)

    def handle_host_msg(self, msg:NetSIOMsg):
        if msg.id in (NETSIO_COLD_RESET, NETSIO_WARM_RESET):
            info_print("HOST {} RESET".format("COLD" if msg.id == NETSIO_COLD_RESET else "WARM"))
            # # clear I/O queues on emulator cold / warm reset
            # debug_print("CLEAR HOST QUEUE")
            # clear_queue(self.host_queue)

        # send message down to connected peripherals
        self.device_manager.to_peripheral(msg)

    def handle_host_msg_sync(self, msg:NetSIOMsg) ->int:
        if msg.id == NETSIO_DATA_BLOCK:
            self.handle_host_msg(msg) # send to devices
            debug_print("< SYNC ATD +{:.0f} {:02X}".format(msg.elapsed() * 1.e6, ATDEV_EMPTY_SYNC))
            return ATDEV_EMPTY_SYNC # return no ACK byte
        # handle sync request
        msg.arg += bytes( (self.sync.set_request(msg.id),) ) # append request sn prior sending
        clear_queue(self.host_queue)
        if not self.device_manager.connected():
            # shortcut: no device is connected, set response now
            self.sync.set_response(ATDEV_EMPTY_SYNC, self.sync.sn) # no ACK byte
        else:
            self.handle_host_msg(msg) # send to devices
        result = self.sync.get_response(0.1, ATDEV_EMPTY_SYNC)
        debug_print("< SYNC ATD +{:.0f} {:02X}".format(msg.elapsed() * 1.e6, result))
        return result

    def handle_device_msg(self, msg:NetSIOMsg, device:NetSIOClient):
        if not self.host_ready.is_set():
            # discard, host is not connected
            return

        # handle sync request/response
        req, sn = self.sync.check_request()
        if req is not None:
            if msg.id == NETSIO_SYNC_RESPONSE and msg.arg[0] == sn:
                # we received response to current SYNC request
                if msg.arg[1] == NETSIO_EMPTY_SYNC:
                    # empty response, no ACK/NAK
                    self.sync.set_response(ATDEV_EMPTY_SYNC, sn) # no ACK byte
                else:
                    # response with ACK/NAK byte and sync write size
                    self.host_handler.clear_rtr()
                    self.sync.set_response(msg.arg[2] | (msg.arg[3] << 8) | (msg.arg[4] << 16), sn)
                return
            elif msg.id in (NETSIO_DATA_BYTE, NETSIO_DATA_BLOCK):
                debug_print("discarding", msg)
                return
            else:
                debug_print("passed", msg)

        if msg.id == NETSIO_SYNC_RESPONSE and msg.arg[1] != NETSIO_EMPTY_SYNC:
            # TODO 
            # host is not interested into this sync response
            # but there is a byte inside response, deliver it as normal byte to host ...
            debug_print("replace", msg)
            msg.id = NETSIO_DATA_BYTE
            msg.arg = bytes( (msg.arg[2],) )

        if self.host_queue.full():
            debug_print("host queue FULL")
        else:
            debug_print("host queue [{}]".format(self.host_queue.qsize()))

        self.host_queue.put(msg)


# workaround for calling parse_args() twice
def get_arg_parser(full=True):
    arg_parser = argparse.ArgumentParser(description = 
            "Connects NetSIO protocol (SIO over UDP) talking peripherals with "
            "NetSIO Altirra custom device (localhost TCP).")
    arg_parser.add_argument('--netsio-port', type=int, default=NETSIO_PORT, help='Change UDP port used by NetSIO peripherals (default {})'.format(NETSIO_PORT))
    arg_parser.add_argument('-d', '--debug', dest='debug', action='store_true', help='Print debug output')
    if full:
        arg_parser.add_argument('--port', type=int, default=NETSIO_ATDEV_PORT, help='Change TCP port used by Altirra NetSIO custom device (default {})'.format(NETSIO_ATDEV_PORT))
        arg_parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Log emulation device commands')
    return arg_parser


def _debug_print(*argv, **kwargs):
    print("{}".format(datetime.now().strftime("%H:%M:%S.%f")), *argv, **kwargs)


def info_print(*argv, **kwargs):
    print("{}".format(datetime.now().strftime("%H:%M:%S.%f")), *argv, **kwargs)


def main():
    global debug_print

    print_banner()

    socketserver.TCPServer.allow_reuse_address = True
    args = get_arg_parser().parse_args()
    
    if args.debug:
        debug_print = _debug_print

    device_manager = NetSIOManager(args.netsio_port)
    host_manager = AtDevManager(get_arg_parser(False))
    hub = NetSIOHub(device_manager, host_manager)

    try:
        hub.run()
    except KeyboardInterrupt:
        print("\nStopped from keyboard")

if __name__ == '__main__':
    main()