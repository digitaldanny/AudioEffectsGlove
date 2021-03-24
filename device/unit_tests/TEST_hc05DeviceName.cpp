#include "build_switches.h"
#if ENABLE_UNIT_TEST_HC05_DEVICE_NAME

#include "unit_tests.h"
#include "wireless_api.h"
#include "uart_if.h"

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
    char msg[] = "AT+NAME?\r\n";
    char* rxBuf;
    char expected[] = "+NAME:HC-05\r\n";
    int lenExpectedMsg = 13;
    bool passed = false;

    // Initialize HW modules
    initExternalHwPower();

    // Enable HC-05 CMD mode, which requires powering off the HC05 module first.
    setExternalHwPower(false);
#if TARGET_HW_MSP432
    WirelessApi::MSP432::SetMode(HC05MODE_CMD);
#else
    while(1); /* Setting cmd mode not implemented for other HW targets */
#endif // TARGET_HW_MSP432
    setExternalHwPower(true);

    while(1)
    {
        // Send the AT command to the HC-05
        if (!Uart::send(msg))
        {
            /* UART send failed */
            while(1);
        }

        // Read back the HC-05 module response
        if (!Uart::recv(&rxBuf))
        {
            /* UART send failed */
            while(1);
        }

        // Check that the rx buffer matches the expected output (test passed?)
        for (int i = 0; i < lenExpectedMsg; i++)
        {
            if (rxBuf[i] != expected[i])
            {
                passed = false;
            }
            else
            {
                passed = true;
            }
        }

        delayMs(1);
    }
}

#endif // ENABLE_UNIT_TEST_MUX_LP
