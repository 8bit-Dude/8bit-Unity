import socket

UDP_IP = "127.0.0.1"
UDP_PORT = 5000

REQUEST_INFO = chr(1)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
print "Listening on:   ", UDP_IP, ":", UDP_PORT

while True:
    packet, addr = sock.recvfrom(256) # buffer size is 256 bytes
    if packet == REQUEST_INFO:
        print "Received request..."
        info = "8bit-unity is easy!\n"
        data = ''.join([REQUEST_INFO, chr(len(info)), info])
        sock.sendto(data, addr)
        print ".. sent information"