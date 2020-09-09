'''
CLASS DESCRIPTION:
The server can accept 1 client connection and send/receive
string messages using TCP/IP. The client can choose to
disconnect at any time.
'''

import socket
import sys
import json

class ServerSocket:

    def __init__(self, port):
        self.host           = ''
        self.port           = port
        self.sock_conn      = None
        self.sock_server    = None

    def __enter__(self):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: __enter__
        Sets up a server socket and returns a connection to the client if one is found.
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        self.sock_server    = self.__setupServerSocket() # Setup the server socket from the port number provided
        self.sock_conn      = self.openConnection() # Setup connection 
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: __exit__
        Closes connection to client and closes server socket.
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        if self.sock_conn:      self.sock_conn.close()
        if self.sock_server:    self.sock_server.close()

    def __setupServerSocket(self):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: __setupServerSocket
        This function sets up a socket on localhost using the port provided upon 
        object instantiation.

        SOCKET DETAILS:
        Protocol        : TCP
        Address Family  : IPv4
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        # AF_INET = address family, IPv4 requires (host, port) tuple
        # SOCK_STREAM = socket expects TCP packets
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print("Server socket created.")

        # Bind the socket to an address so the server
        # can accept client connections.
        try:
            s.bind((self.host, self.port))
        except socket.error as msg:
            print(msg)
        print("Socket bind complete")

        print("Host name: {}".format(socket.gethostbyname(socket.gethostname())))
        return s

    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    # PUBLIC METHODS
    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+

    def openConnection(self):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: openConnection
        Listens for a client connection on the self.server_socket. If a connection is
        found, it is returned.
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        # Allows 1 connection.
        self.sock_server.listen(1)

        # Create a new socket "connection" that allows send/receive
        # from the address tied to the other side of the socket.
        connection, address = self.sock_server.accept()
        print("Connected to: " + address[0] + ":" + str(address[1]))

        # Assign the socket connection to the class instance's attributes
        self.sock_conn = connection
        return connection

    def recvRequest(self):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: recvRequest
        Waits for a request over the client connection and returns the raw data.
        This function does NOT decode the received request.

        INPUTS: N/A

        RETURN: 
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        if not self.sock_conn: return None
        data = self.sock_conn.recv(1024)
        return data.decode('utf-8')

    def sendResponse(self, data):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: sendResponse
        Sends raw data over connection.

        INPUTS:
        @data - ()

        RETURN: 
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        if not self.sock_conn: return False
        encoded = data.encode('utf-8')
        self.sock_conn.send(encoded)
        return True

# ==============================================================
# ==============================================================
# ==                                                          ==
# ==                 STAND ALONE TEST MODULES           
# ==                                                          ==
# ==============================================================
# ==============================================================

def TestProtocol():
    while True:
        with ServerSocket(5560) as s:
            request = s.recvRequest()
            print("Received request: {}".format(request))

if __name__ == "__main__":
    print(sys.version)
    TestProtocol()
