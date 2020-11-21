import socket

IP = "127.0.0.1"
PORT = 1234

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Send message to server
sock.sendto("8bit-Unity is easy!", (IP, PORT))

# Wait for reply
packet = sock.recvfrom(256)
print 'Echoed from UDP server: "%s"' % packet[0]