using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * 
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 */
public class BTMessageListener : MonoBehaviour
{
    [SerializeField] BTMessageHandler messageHandler;

    // Invoked when a line of data is received from the serial device.
    void OnMessageArrived(string msg)
    {
        Debug.Log("Received: " + msg);
        messageHandler.Recv(msg);
    }

    // Invoked when a connect/disconnect event occurs. The parameter 'success'
    // will be 'true' upon connection, and 'false' upon disconnection or
    // failure to connect.
    void OnConnectionEvent(bool success)
    {
        if (success)
            Debug.Log("Connected");
        else
            Debug.Log("Disconnected");
    }
}
