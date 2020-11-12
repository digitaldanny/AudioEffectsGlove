using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BTMessageHandler : MonoBehaviour
{
    [SerializeField] SerialController serialController;
    [SerializeField] string received;

    public bool Send(string msg)
    {
        serialController.SendSerialMessage(msg);
        return true;
    }

    public bool Recv(string msg)
    {
        this.received = msg;
        return true;
    }
}
