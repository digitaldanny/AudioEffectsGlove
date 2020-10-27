/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: SocketResource.cs
 * This class is designed to directly receive data from the RasPi through the socket connection instead
 * of communicating to the local Python scripts (drivers.py). This reduces controller latency in the 
 * simulation and add to the system's portability because it requires fewer third party tools.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

public class SocketResource : MonoBehaviour
{
    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * ENUMS
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    private enum Handshake : int
    {
        READY_TO_SEND = 0,
        READY_TO_RECV = 1
    }
    
    private enum SensorAxis : int
    {
        X = 0,
        Y = 1,
        Z = 2
    }

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * CLASSES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    public class SensorData
    {
        public float x;
        public float y;
        public float z;

        public SensorData()
        {
            x = 0f;
            y = 0f;
            z = 0f;
        }
    }

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROPERTIES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    // Data received from the device.
    private Queue<String>[] sensorDataToParse;      // holds the strings of received data from the device in json.dump format
    public Queue<float>[,] sensorDataQueues;    // 2D array of queues for holding the float data

    // socket variables
    private TcpClient socketConnection;
    private Thread clientReceiveThread;
    private Thread parserThread;

    private int port;   // remote host port to make connection on
    private bool connected;
    private string serverIP;
    private Handshake handshake; // synchronization between sensor request and listener thread

    // messages to send to the server
    private string accel;
    private string gyro;
    private string mag;
    private string exit;
    private string kill;

    // json related parameters
    SensorData jsonDataObject;

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * METHODS
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    void Start()
    {
        // initialize the buffers for parsing the received data
        sensorDataToParse = new Queue<string>[3];
        for (int i = 0; i < sensorDataToParse.GetLength(0); i++)
        {
            sensorDataToParse[i] = new Queue<string>();
        }

        // initialize each of the received data queues in the 2D array of queues.
        sensorDataQueues = new Queue<float>[3, 3]; // 3 sensors, 3 axes each
        for (int row = 0; row < sensorDataQueues.GetLength(0); row++)
        {
            for (int col = 0; col < sensorDataQueues.GetLength(1); col++)
            {
                sensorDataQueues[row, col] = new Queue<float>();
            }
        }

        connected   = false;
        serverIP    = "192.168.1.";    // the rest of the address will be defined in the FindServerIP function
        port        = 5560;

        // set the messages from the configurations filev
        accel = "0";
        gyro = "1";
        mag = "2";
        exit = "E";
        kill = "K";

        // json related
        jsonDataObject = new SensorData();

        // Start trying to make a connection to the server
        // and initialize the background thread that parses the 
        // json string queue.
        InitParserThread();
        StartCoroutine(ConnectToServerCoroutine());
    }

    void Update()
    {
        // Once the ConnectToServerCoroutine is successful, sending requests to the server is safe.
        if (connected)
        {
            SendSensorRequests();
        }
    }

    /*
     *  If there is no connection to the server, try to make a connection. 
     *  Once a connection is made, stop the coroutine.
     */
    private IEnumerator ConnectToServerCoroutine()
    {
        // before making a connection to the server, find its IP address.
        while (!FindServerIP())
        {
            yield return new WaitForSeconds(.5f);
        }

        // Wait for the server to stabilize from client disconnection before trying to reconnect.
        yield return new WaitForSeconds(.5f);
        Debug.Log("Making connection to the IP address: " + serverIP);
        ConnectToTcpServer();                   // make a socket connection to the server.

        Debug.Log("Connected to the server correctly");
        handshake = Handshake.READY_TO_SEND;    // enable sending sensor requests to the server.  
        connected = true;                       // allow the client to start sending requests.
    }

    /*
     *  SUMMARY:
     *  Set up a socket connection to the server. Searches through IP addresses on 
     *  the configured port automatically until a connection is found.
     */
    private void ConnectToTcpServer()
    {
        try
        {
            clientReceiveThread = new Thread(new ThreadStart(ListenForData));
            clientReceiveThread.IsBackground = true;
            clientReceiveThread.Start();
        }
        catch (Exception e)
        {
            Debug.Log("On client connect exception " + e);
        }
    }

    /*
    * SUMMARY: Search for the correct server IP and assign it to the global.
    * RETURN: true = connection was made, false = connection was not found
    */
    private bool FindServerIP()
    {
        bool connectionSuccessful = false;

        // Test all local area network (LAN) IPs
        for (int ping = 0; ping < 255; ping++)
        {
            string pingIP = serverIP + ping.ToString();
            Debug.Log("Pinging: " + pingIP);

            try
            {
                // Try connecting to the IP with a timeout of .05 seconds
                socketConnection = new TcpClient();
                if (!socketConnection.ConnectAsync(pingIP, port).Wait(150))
                {
                    continue; // No connection at this address
                }

                // allows reconnection for synchronous in ListenForData
                Send(exit);                 // tell the server to close connection
                socketConnection.Close();   // close connection on client side
                serverIP = pingIP;
                connectionSuccessful = true;
                break;
            }
            catch
            {
                Debug.Log("Connection Failure");
            }
        }

        return connectionSuccessful;
    }

    /*
     *  SUMMARY:
     *  Background thread for catching messages from the server.
     */
    private void ListenForData()
    {
        try
        {
            // Tries to create a synchronous tcp socket
            socketConnection = new TcpClient(serverIP, port);
            Byte[] bytes = new Byte[1024];
            int queueNum = 0;

            while (true)
            {
                // Get a stream object for reading 				
                using (NetworkStream stream = socketConnection.GetStream())
                {
                    int length;

                    // Read incomming stream into byte arrary. The first portion of the message
                    // is the length that should be read for one sensor.
                    while ((length = stream.Read(bytes, 0, 2)) != 0)
                    {
                        var incomingData = new byte[length];
                        Array.Copy(bytes, 0, incomingData, 0, length);
                        string serverMessageLength = Encoding.UTF8.GetString(incomingData); // Convert byte array to string message. 

                        // convert the received message length to an integer value
                        Debug.Log("Length to parse: " + serverMessageLength);
                        int messageLength = Int32.Parse(serverMessageLength);
                        Debug.Log("Length parsed: " + messageLength);

                        // Read in the rest of the message based on the length specified

                        int jsonLength = stream.Read(bytes, 0, messageLength);
                        var jsonMessage = new byte[jsonLength];
                        Array.Copy(bytes, 0, jsonMessage, 0, jsonLength);
                        string serverMessage = Encoding.UTF8.GetString(jsonMessage); // Convert byte array to string message. 

                        // Send the data to the string queue to be parsed and spread out to 
                        // the individual sensor data queues.
                        Debug.Log("SERVER MESSAGE: " + serverMessage + " saving in buffer #" + queueNum);
                        sensorDataToParse[queueNum].Enqueue(serverMessage);

                        // If received data has already been sent to this sensor, go to the 
                        // next sensor and store the received strings into that. If all sensors
                        // have been filled, break out of the loop and  allow the client to send
                        // another request.
                        if (queueNum == 2)
                        {
                            Debug.Log("Received data for all sensors. Resetting to first buffer.");
                            queueNum = 0;
                        }
                        else
                        {
                            Debug.Log("Going to the next sensor for parsing data");
                            queueNum++;
                        }
                    }
                }
            }
        }
        catch (SocketException socketException)
        {
            Debug.Log("Socket exception: " + socketException);
        }
    }

    /*
     * 
     * SUMMARY: 
     * Initialize the parser thread in the background.
     */
    private void InitParserThread()
    {
        // Start up the parser thread
        parserThread = new Thread(new ThreadStart(ParserThread));
        parserThread.IsBackground = true;
        parserThread.Start();
    }

    /*
     *  SUMMARY:
     *  Thread to parse all buffer data to place into the sensor Queues.
     */
    private void ParserThread()
    {
        int sensorIndex = 0;

        while (true)
        {
            // only try storing the next sensor's data if the current
            // sensor's data successfully parsed.
            if (Parse(sensorIndex))
                sensorIndex++;

            // reset the sensor index to 0 if it goes past
            if (sensorIndex == 3)
                sensorIndex = 0;
        }
    }

    /*
     *  SUMMARY:
     *  Dequeue and parse the data stored in the ParsedQueue for each sensor and 
     *  store into the SensorQueue arrays in floating point format.
     *  
     *  RETURN:
     *  newDataReceived - (bool) True if new data was parsed and stored into the data queue.
     */
    private bool Parse(int sensorIndex)
    {
        bool newDataReceived = false;

        if (sensorDataToParse[sensorIndex].Count > 0)
        {
            // parse the string and store the data into the jsonDataObject instance.
            string parseMe = sensorDataToParse[sensorIndex].Dequeue();
            JsonUtility.FromJsonOverwrite(parseMe, jsonDataObject);

            // Write the data out to console for debugging.
            Debug.Log("X: " + jsonDataObject.x);
            Debug.Log("Y: " + jsonDataObject.y);
            Debug.Log("Z: " + jsonDataObject.z);

            // store the float values to the sensor data queues
            sensorDataQueues[sensorIndex, (int)SensorAxis.X].Enqueue(jsonDataObject.x);
            sensorDataQueues[sensorIndex, (int)SensorAxis.Y].Enqueue(jsonDataObject.y);
            sensorDataQueues[sensorIndex, (int)SensorAxis.Z].Enqueue(jsonDataObject.z);

            newDataReceived = true;
        }

        return newDataReceived;
    }

    /*
     * This function sends the sensor requests to the server and waits for 
     * a response before requesting the next sensor value.
     */
    private void SendSensorRequests()
    {
        // Request sensor data
        if (handshake == Handshake.READY_TO_SEND)
        {
            Send(accel);
            Send(mag);
            Send(gyro);
            //handshake = Handshake.READY_TO_RECV;  // wait for the listener's handshake
        }
    }

    /*
     * SUMMARY:
     * Send request to server over the socket connection established with ConnectToTcpServer
     */
    private void Send(string clientRequest)
    {
        if (socketConnection == null)
            return;

        try
        {
            // Get a stream object for writing. 			
            NetworkStream stream = socketConnection.GetStream();
            if (stream.CanWrite)
            {
                // Convert string message to byte array.    
                // byte[] clientMessageAsByteArray = Encoding.ASCII.GetBytes(clientMessage);
                byte[] clientMessageAsByteArray = Encoding.UTF8.GetBytes(clientRequest);

                // Write byte array to socketConnection stream.                 
                stream.Write(clientMessageAsByteArray, 0, clientMessageAsByteArray.Length);
                Debug.Log("Client sent his message - should be received by server");
            }
        }
        catch (SocketException socketException)
        {
            Debug.Log("Socket exception: " + socketException);
        }
    }

    /*
     * SUMMARY:
     * Send request to server to end the socket connection. The client requests to disconnect
     * from the server.
     */
    void OnApplicationQuit()
    {
        Debug.Log("Application ending after " + Time.time + " seconds");

        // EXIT signals server that the connection socket should be closed.
        try
        {
            Send(exit);
        }
        catch
        {
            Debug.Log("No connection available.");
        }

        // Stop the listener thread so that it's not trying
        // to receive data from the server when the connection
        // socket is closed.
        clientReceiveThread.Abort();

        // Clean close the socket on client side.
        try
        {
            socketConnection.GetStream().Close();   // close stream
            socketConnection.Close();               // close connection
        }
        catch
        {
            Debug.Log("No connection available.");
        }
    }
}