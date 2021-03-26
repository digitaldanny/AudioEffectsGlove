#include "build_switches.h"
#if ENABLE_UNIT_TEST_HC05_RW_TO_SLAVE

#include "entry_points.h"
#include "hc05_api.h"
#include <string.h>

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_hc05RwToSlave
 * This test uses the master HC-05 connected to the MSP432 to read from / write
 * to the slave HC-05 connected to the C2000 dev board.
 *
 * The test also uses a payload that resembles the length of the payload
 * the final demo will be using (150-200 bits).
 *
 * NOTES:
 * This test expects the user to have the MSP432 connected to the HC-05 module
 * according to the UART pin connections defined in the systemIO variable.
 *
 * This test also utilizes the Pch MOSFET power switch, so the external power
 * GPIO connection defined in the systemIO variable must be connected.
 *
 * This test also uses the bluetooth enable defined in the systemIO variable
 * for switching between the HC-05's data/CMD mode.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_hc05RwToSlave()
{
    char* rxBuf;

    // Messages to send
#define NUM_PAYLOADS 1
    char payloadSmall[] = "Hello, world!! 0123456789\n"; // 25 bytes = 200-bit transfer (final character always ignored)
    char *payloads[NUM_PAYLOADS] = { payloadSmall };

    // Response expected from HC-05 for message received.
    char expected[] = "OK\n";
    uint16_t lenExpectedMsg = 3;

    // Initialize HW modules
    initExternalHwPower();

    // Enable HC-05 DATA mode (NOTE: This will power cycle the external hardware).
    if (!Hc05Api::SetMode(HC05MODE_DATA))
    {
        // Trap CPU - HC05 init failed
        while (1);
    }

    for (int i = 0; i < NUM_PAYLOADS; i++)
    {
        // Send the AT command to the HC-05
        if (!Hc05Api::Send(payloads[i]))
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
        for (uint16_t i = 0; i < lenExpectedMsg; i++)
        {
            if (rxBuf[i] != expected[i])
            {
                // FAILED TEST
                return -1;
            }
        }
        delayMs(1);
    }
    return 0;
}

#endif // ENABLE_UNIT_TEST_HC05_RW_TO_SLAVE
