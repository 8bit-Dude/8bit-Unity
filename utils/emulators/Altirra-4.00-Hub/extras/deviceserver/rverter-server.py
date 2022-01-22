# Sample device server - basic R-Verter
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

import deviceserver
import socketserver
import threading

rv_recv = None
rv_send = None

class RVerterCommHandler(socketserver.BaseRequestHandler):
    def handle(self):
        global rv_send

        rv_send = self.send_byte

        print("Comm connection received")

        try:
            while True:
                v = self.request.recv(1)

                if len(v) == 0:
                    break

                if rv_recv is None:
                    print("Discarding comm: {:02X}".format(v[0]))
                else:
                    rv_recv(v[0])
                
        finally:
            rv_send = None

        print("Comm connection closed")

    def send_byte(self, c):
        self.request.sendall(bytes([c]))

class RVerterCommThread(threading.Thread):
    def __init__(self, port: int):
        super().__init__()

        self.port = int(port)

    def run(self):
        with socketserver.TCPServer(('', self.port), RVerterCommHandler) as server:
            print("Waiting for comm connection on port {}".format(self.port))

            server.serve_forever()

class RVerterServer(deviceserver.DeviceTCPHandler):
    def handle_script_post(self, ch: int, cycles_per_bit: int, timestamp: int) -> int:
        if ch == 256:
            # special case to notify that a new byte can be received
            self.recv_ready_cond.acquire()
            self.recv_ready = True
            self.recv_ready_cond.notify()
            self.recv_ready_cond.release()
        elif ch == 257:
            # special case to notify that a reset has occurred
            print("Enabling transfers")
            self.recv_ready_cond.acquire()
            self.recv_enabled = True
            self.recv_ready = True
            self.recv_ready_cond.notify()
            self.recv_ready_cond.release()
        elif ch == 258:
            # device is off - discard incoming bytes
            print("Disabling transfers")
            self.recv_ready_cond.acquire()
            self.recv_enabled = False
            self.recv_ready_cond.notify()
            self.recv_ready_cond.release()
        elif ch < 256:
            # plain data byte sent from POKEY
            if rv_send is not None:
                print("Received byte: {:02X} ({} cycles)".format(ch, cycles_per_bit))
                rv_send(ch)
            else:
                print("Discarding received byte: {:02X} ({} cycles)".format(ch, cycles_per_bit))

    def handle(self):
        global rv_recv

        self.recv_ready = False
        self.recv_ready_cond = threading.Condition()
        self.recv_enabled = False
        rv_recv = self.recv_byte

        try:
            super().handle()
        finally:
            self.recv_ready_cond.acquire()
            self.recv_enabled = False
            self.recv_ready_cond.notify()
            self.recv_ready_cond.release()
            rv_recv = None

    def recv_byte(self, ch):
        self.recv_ready_cond.acquire()
        while True:
            if not self.recv_enabled:
                self.recv_ready_cond.release()
                print("Discarding byte: {:02X} (1200 baud)".format(ch))
                break

            if not self.recv_ready:
                self.recv_ready_cond.wait()
                continue

            self.recv_ready = False
            self.recv_ready_cond.release()

            print("Sending byte: {:02X} (1200 baud)".format(ch))
            # send at 1200 baud for now
            self.req_interrupt(ch, 1492)
            break

    def serve_forever(self):
        print("serving!")
        super.serve_forever()

def rv_run(server):
    comm_thread = RVerterCommThread(6507)
    comm_thread.start()

    server.serve_forever()

if __name__ == '__main__':
    deviceserver.run_deviceserver(RVerterServer, run_handler = rv_run)
