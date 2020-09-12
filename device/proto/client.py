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
        
    def __init__(self, port, timeout):
        self.port = port
        self.timeout = timeout
        self.conn = None

    def __enter__(self):
        print("Setting up client connection")
        self.conn = self.__setupConnection()
        return self
        
    def __exit__(self, exception_type, exception_value, traceback):
        print("Closing client connection")
        if self.conn: self.conn.close()
    
    def __setupConnection(self):
        ''' 
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: __setupConnection 
        Search all LAN IPs for a valid connection on the defined port.

        SOCKET DETAILS:
        Protocol        : TCP
        Address Family  : IPv4
        
        RETURN: (socket.socket) connection socket to host.
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        sock = None
        #serverIP = "192.168.0."
        serverIP = "127.0.1."

        # Set up socket for communicating to server with IPv4, TCP/IP
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(self.timeout)
  
        # Test all 256 LAN IPs and return the socket if
        # a valid connection is made.
        for ping in range(0,255): 
            address = serverIP + str(ping) 
            
            # Try to connect to the port at this address. If a timeout
            # occurs, this was not the server's IP.
            try:
                sock.connect((address, self.port)) # Timeout reset
                sock.settimeout(None)
                return sock
                
            except:
                # Only update the user occasionally
                if ping % 100 == 0:
                    print("Searching for connection..")
        return None # connection failed

    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    # PUBLIC METHODS
    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+

    def request(self, req):

        # send the request to the server as a single byte
        self.conn.send(req.encode('utf-8'))

        # receive the data from the server
        #msgLength = self.conn.recv(1024)
        #data = self.conn.recv(int(msgLength))

        # decode the json.dump
        #decoded = json.loads(data.decode('utf-8'))
        #x = decoded.get("x")
        #y = decoded.get("y")
        #z = decoded.get("z")

        #return x,y,z

# ==============================================================
# ==============================================================
# ==                                                          ==
# ==               STAND ALONE TEST MODULES           
# ==                                                          ==
# ==============================================================
# ==============================================================

def protocol():

    while True:
        with ClientSocket(9876, .05) as c:
            if not c:
                print("Trying again in 2 seconds.\n")
                time.sleep(2) 
                continue
            req = input("Enter a message for the server: $")
            c.request(req)

if __name__ == "__main__":
    print(sys.version)
    protocol()