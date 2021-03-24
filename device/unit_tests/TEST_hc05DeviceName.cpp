#include "build_switches.h"
#if ENABLE_UNIT_TEST_HC05_DEVICE_NAME

#include "unit_tests.h"
#include "hc05_api.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_hc05DeviceName
 * This test expects the user to have the MSP432 connected to the HC-05 module
 * according to the UART pin connections defined in the systemIO variable.
 *
 * This test also utilizes the Pch MOSFET power switch, so the external power
 * GPIO connection defined in the systemIO variable must be connected.
 *
 * This test also uses the bluetooth enable defined in the systemIO variable
 * for switching between the HC-05's data/CMD mode.
 *
 * This test reads back from the HC-05 "NAME" register using the AT command
 * "AT+NAME?" and expects the device to return "HC-05\n"
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_hc05DeviceName()
{
    char* rxBuf;

    char msg[] = "AT+NAME?\r\n";
    char expected[] = "+NAME:HC-05\r\n";
    int lenExpectedMsg = 13;

    // Initialize HW modules
    initExternalHwPower();

    // Enable HC-05 CMD mode (NOTE: This will power cycle the external hardware).
    if (!Hc05Api::SetMode(HC05MODE_CMD))
    {
        // Trap CPU - HC05 init failed
        while (1);
    }

    while(1)
    {
        // Send the AT command to the HC-05
        if (!Hc05Api::Send(msg))
        {
            /* UART send failed */
            while(1);
        }

        // Read back the HC-05 module response
        if (!Hc05Api::Recv(&rxBuf))
        {
            // UART send failed
            while(1);
        }

        // Check that the rx buffer matches the expected output (test passed?)
        for (int i = 0; i < lenExpectedMsg; i++)
        {
            if (rxBuf[i] != expected[i])
            {
                // FAILED TEST
                return -1;
            }
            else
            {
                // PASSED TEST
                return 0;
            }
        }
        delayMs(1);
    }
}

#endif // ENABLE_UNIT_TEST_MUX_LP
