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
        
        self.stored         = "DEADBEEF"
        self.host           = ''
        self.port           = port
        self.sock_conn      = None
        self.sock_server    = self.__setupServer() # Setup the socket from the port number provided

    def __getServerSocketInfo(self):
        return self.host, self.port

    def __setupServer(self):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: __setupServer
        
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        # AF_INET = address family, APv4 requires (host, port) tuple
        # SOCK_STREAM = socket expects TCP packets
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print("Socket created.")

        # Bind the socket to an address so the server
        # can accept client connections.
        try:
            serverSockInfo = self.__getServerSocketInfo()
            s.bind(serverSockInfo)
        except socket.error as msg:
            print(msg)
        print("Socket bind complete")

        return s

    def __READ(self):
        reply = self.stored
        return reply

    def __WRITE(self, message):
        self.stored = message[1]

    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    # PUBLIC METHODS
    # +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+

    def setupConnection(self):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: SetupConnection
        
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

    def decodeRequest(self):
        '''
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        DESCRIPTION: decodeRequest
        
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
        '''
        closeConnection = False

        # RECEIVE THE DATA ------------------------------------
        #start = timeit.default_timer()
        data = self.sock_conn.recv(1024)
        data = data.decode('utf-8')

        # Split the data to separate the command from theWW
        # rest of the data.
        dataMessage = data.split(' ', 1)
        command = dataMessage[0]

        # Decode the command received from the client ----------
        
        # Send the currently stored data back to the client.
        # This may not be used once the FIFOs are implemented and
        # a periodic status updater will be sent back to the client.
        if command == 'READ':
            print("Reading from memory..")
            reply = self.__READ()

        # writes the data to the test variable.
        # Soon this will be updated to write to the FIFO
        elif command == 'WRITE':
            print("Storing to memory..")
            self.__WRITE(dataMessage)
            reply = 'Updating the server with new data..'

        # Repeat the previous data message.
        elif command == 'REPEAT':
            reply = self.__REPEAT(dataMessage)

        # Go back to the beginning of the loop
        # and listen for another client connection.
        elif command == 'EXIT':
            print("Client has exited..")
            closeConnection = True

        # Shut down the server by closing the SERVER socket.
        # End of the program.
        elif command == 'KILL':
            print("Server socket closed..")
            self.sock_server.close()
            closeConnection = True

        # Handle unknown commands.
        else:
            reply = 'Unknown command..'

        # RESPOND TO CLIENT --------------------------------
        # Only respond to client if the client is still connected.
        if not closeConnection:
            self.sock_conn.sendall(str.encode(reply))
            print("Data has been send..")  

        # Else if the server received exit or kill data, close the
        # CONNECTION socket to the client.
        else:
            self.sock_conn.close()
            print("Client socket closed..")

        # if the client has requested to exit OR kill the server
        return closeConnection

class Server(ServerSocket):

    def __init__(self, port):
        ServerSocket.__init__(self, port)
        
    def __exit__(self):
        self.closeConnection()
        self.closeServer()
        
    '''SUMMARY: Try to close the server socket. '''
    def closeConnection(self):
        try:
            self.sock_conn.close()
            print("Connection socket closed.")
        except:
            print("No connection socket to close.")
            
    ''' SUMMARY: Try to close the connection socket. '''
    def closeServer(self):
        try:
            self.sock_server.close()
            print("Server socket closed.")
        except:
            print("No server socket to close.")

    ''' SUMMARY: Receive a request from the user - cfg.ACCEL, cfg.GRYO, cfg.MAG'''
    def receive(self):
        req = self.sock_conn.recv(1)
        return req.decode('utf-8')

    def respond(self, msg):

        # First send the length of the message, then send the
        # message. The client will be able to read the length
        # and then pull the appropriate amount of data.
        encoded = msg.encode('utf-8')
        self.sock_conn.send(str(len(encoded)).encode('utf-8'))
        self.sock_conn.send(encoded)

# ==============================================================
# ==============================================================
# ==                                                          ==
# ==                 STAND ALONE TEST MODULES           
# ==                                                          ==
# ==============================================================
# ==============================================================

def TestProtocol():

    # instantiate the wifi server which automatically
    # sets up a server socket on the specified port.
    s = Server(5560)

    mess = None

    # Continue to make connections or send/receive data
    # until a client requests that the server socket is
    # closed.
    while True:
        try:
            s.setupConnection()
            close_conn = False
            
            while not close_conn:
                mess = s.receive()
                print("REQ: " + str(mess))

                # determine if the request is asking to disconnect
                # or shut down the server.
                if mess == 'E' or mess == 'K': break

                # return the appropriate data to the client.
                s.respond('{"x" : 2.561, "y" : 1.231, "z" : 4.322}')

            if mess == 'K': break 
        except Exception as e:
            print(e)
            break    

if __name__ == "__main__":
    print(sys.version)
    TestProtocol()
