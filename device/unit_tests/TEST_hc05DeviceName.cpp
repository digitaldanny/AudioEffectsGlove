#include "build_switches.h"
#if ENABLE_UNIT_TEST_HC05_DEVICE_NAME

#include "unit_tests.h"
#include "uart_if.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_hc05DeviceName
 * This test expects the user to have the MSP432 connected to the HC-05 module
 * according to pin connections defined in uart_if.cpp.
 *
 * This test reads back from the HC-05 "NAME" register using the AT command
 * "AT+NAME?" and expects the device to return "HC-05\n"
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_hc05DeviceName()
{
    char msg[] = "AT+NAME?\r\n";
    char rxBuf[100] = {0};
    char expected[] = "HC-05\r\n";
    int lenExpectedMsg = 6;

    if (!Uart::init())
    {
        /* UART initializations failed */
        while(1);
    }

    // Send the AT command to the HC-05
    if (!Uart::send(msg))
    {
        /* UART send failed */
        while(1);
    }

    delayMs(1);

    // Read back the HC-05 module response
    if (!Uart::recv(rxBuf))
    {
        /* UART send failed */
        while(1);
    }

    delayMs(1);

    // Check that the rx buffer matches the expected output
    for (int i = 0; i < lenExpectedMsg; i++)
    {
        if (rxBuf[i] != expected[i])
            return -1;
    }

    return 0; // rx matches expected output
}

#endif // ENABLE_UNIT_TEST_MUX_LP
