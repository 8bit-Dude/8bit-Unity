import socket

IP = "127.0.0.1"
PORT = 1234

# Create and bind UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IP, PORT))
print "Listening on UDP port: ", IP, ":", PORT

while True:
    # Wait for incoming packet
    print 'waiting for a packet'
    packet, addr = sock.recvfrom(256) # buffer size is 256 bytes
    
    # Echo packet back to client
    if packet:
        print 'packet from', addr
        print 'received: "%s"' % packet
        sock.sendto(packet, addr)
        print 'data echoed back to client'
