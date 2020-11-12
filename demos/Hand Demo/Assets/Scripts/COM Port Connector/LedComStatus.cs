using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LedComStatus : MonoBehaviour
{
    [SerializeField] bool isComPortConnected;
    [SerializeField] Color colorConnected;
    [SerializeField] Color colorDisconnected;

    private Image imageLed;

    void Start()
    {
        isComPortConnected = false;
        imageLed = GetComponent<Image>();
    }

    void Update()
    {
        UpdateLedColor(); // @todo - this should be called externally by the bluetooth object
    }

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: UpdateLedColor
     * This function updates the color of the COM Port Status Led image based
     * on whether the script is connected to a COM port.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     */
    private void UpdateLedColor()
    {
        if (this.isComPortConnected)
            imageLed.color = colorConnected;
        else
            imageLed.color = colorDisconnected;
    }
}
