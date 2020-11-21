import socket

IP = "127.0.0.1"
PORT = 1234

# Create and bind TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((IP, PORT))
sock.listen(1)
print "Listening on TCP port: ", IP, ":", PORT

while True:
    # Wait for a connection
    print 'waiting for a connection'
    connection, addr = sock.accept()    
    print 'connection from', addr

    # Echo data back to client
    while True:
        data = connection.recv(256)
        if data:
            print 'received: "%s"' % data
            connection.sendall(data)
            print 'data echoed back to client'
        else:
            break
            
    # Clean up the connection
    connection.close()
