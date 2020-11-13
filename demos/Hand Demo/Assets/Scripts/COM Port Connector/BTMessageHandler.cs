using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BTMessageHandler : MonoBehaviour
{
    [SerializeField] SerialControllerCustomDelimiter serialController;
    [SerializeField] byte[] received;

    public void Send(byte[] msg)
    {
        serialController.SendSerialMessage(msg);
    }

    public void Recv(byte[] msg)
    {
        this.received = msg;
    }
}
