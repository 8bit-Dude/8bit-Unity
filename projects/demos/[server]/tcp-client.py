import socket

IP = "127.0.0.1"
PORT = 1234

# Create a TCP connection
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((IP, PORT))

# Send message to server
sock.sendall("8bit-Unity is easy!")

# Wait for reply
data = sock.recv(256)
sock.close()

print 'Echoed from TCP server: "%s"' % data