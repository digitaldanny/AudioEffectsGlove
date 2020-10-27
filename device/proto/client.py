'''
+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
DESCRIPTION: client.py
This is a client-side script for testing the server.py script.
This will never be used in a real application, but it is useful
for testing how the server will respond to the Unity client.
+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
'''

import socket
import sys
import json
import time

class ClientSocket:
        
    def __init__(self, ipv4, port, timeout):
        self.ip = ipv4
        self.port = port
        self.timeout = timeout
        self.conn = None

    def __enter__(self):
        print("Setting up client connection")
        self.conn = self.__setupConnection()

        if self.conn:
            return self
        else:
            return None
        
    def __exit__(self, exception_type, exception_value, traceback):
        print("Closing client connection")
        if self.conn: self.conn.close()
    
    def __setupConnection(self):
        ''' 
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: __setupConnection 
        Connect to defined ip address through defined port number.

        SOCKET DETAILS:
        Protocol        : TCP
        Address Family  : IPv4
        
        RETURN: (socket.socket) connection socket to host.
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        sock = None
        serverIP = self.ip
        portNum = self.port

        # Set up socket for communicating to server with IPv4, TCP/IP
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(self.timeout)
            
        # Try to connect to the port at this address. If a timeout
        # occurs, this was not the server's IP.
        try:
            print("Connecting to server on {}:{}".format(serverIP, portNum))
            sock.connect((serverIP, portNum)) # Timeout reset
            sock.settimeout(None)
            return sock
            
        except:
            print("Could not connect to server on {}:{}".format(serverIP, portNum))
            return None # connection failed

    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    # PUBLIC METHODS
    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+

    def sendRequest(self, req):
        self.conn.send(req.encode('utf-8'))

    def recvResponse(self):
        dataUtf8Encoded = self.conn.recv(1024)
        data = dataUtf8Encoded.decode('utf-8')
        return data

# ==============================================================
# ==============================================================
# ==                                                          ==
# ==               STAND ALONE TEST MODULES           
# ==                                                          ==
# ==============================================================
# ==============================================================

def protocol():

    while True:
        with ClientSocket("127.0.0.1", 5563, .5) as c:
            
            if not c:
                print("Trying again in 2 seconds.\n")
                time.sleep(2) 
                continue

            # Send request to server
            req = input("Enter a message for the server ('q' to quit): $")
            print("Sending {}".format(req))
            c.sendRequest(req)

            if req == "q":
                print("Exitting infinite loop")
                return

            # Wait for response from server
            print("Waiting for response from server")
            rx = c.recvResponse()
            print("Client received data: {}".format(rx))

if __name__ == "__main__":
    print(sys.version)
    protocol()