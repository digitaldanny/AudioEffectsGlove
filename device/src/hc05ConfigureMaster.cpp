#include "build_switches.h"
#if ENABLE_HC05_CONFIG_MSTR

#include "entry_points.h"
#include "hc05_api.h"
#include <string.h>

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_hc05DeviceName
 * This test configures the master HC-05's role, connection type, and slave
 * address.
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
int unitTest_hc05ConfigMaster()
{
    char* rxBuf;

    // HC-05 configuration registers to read
#define NUM_READS 4
    char rdUart[] = "AT+UART?\r\n"; // Read UART configurations (baud, stop bit, parity)
    char rdMaster[] = "AT+ROLE?\r\n"; // Is role master (1) or slave (0)?
    char rdConnMode[] = "AT+CMODE?\r\n"; // Read connection mode (0=fixed address)
    char rdAddrBind[] = "AT+BIND?\r\n"; // Read fixed slave address
    char *reads[NUM_READS] = { rdUart, rdMaster, rdConnMode, rdAddrBind };

    // HC-05 configurations to send
#define NUM_CONFIGS 3
    char cfgMaster[] = "AT+ROLE=1\r\n"; // Set as master
    char cfgConnMode[] = "AT+CMODE=0\r\n"; // Set connection mode to fixed address
    char cfgAddrBind[] = "AT+BIND=98D3,91,FDED7D\r\n"; // Set connection address to slave HC-05"
    char *configs[NUM_CONFIGS] = { cfgMaster, cfgConnMode, cfgAddrBind };

    // Response expected from HC-05 for each config message
    char expected[] = "OK\r\n";
    uint16_t lenExpectedMsg = 4;

    // Initialize HW modules
    initExternalHwPower();

    // Enable HC-05 CMD mode (NOTE: This will power cycle the external hardware).
    if (!Hc05Api::SetMode(HC05MODE_CMD))
    {
        // Trap CPU - HC05 init failed
        while (1);
    }

    // Read current device configurations
    for (int i = 0; i < NUM_READS; i++)
    {
        uint8_t numBytes = 0;
        while (*(reads[i]+numBytes) != '\n')
        {
            numBytes++;
        }
        numBytes++;

        // Send the AT command to the HC-05
        if (!Hc05Api::Send(reads[i], numBytes))
        {
            /* UART send failed */
            while(1);
        }

        // Read back the HC-05 module response
        if (!Hc05Api::Recv(&rxBuf, lenExpectedMsg))
        {
            // UART send failed
            while(1);
        }
        delayMs(1);
    }

    // Configure the HC-05 device with new settings
    for (int i = 0; i < NUM_CONFIGS; i++)
    {
        uint8_t numBytes = 0;
        while (*(configs[i]+numBytes) != '\n')
        {
            numBytes++;
        }
        numBytes++;

        // Send the AT command to the HC-05
        if (!Hc05Api::Send(configs[i], numBytes))
        {
            /* UART send failed */
            while(1);
        }

        // Read back the HC-05 module response
        if (!Hc05Api::Recv(&rxBuf, lenExpectedMsg))
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

#endif // ENABLE_HC05_CONFIG_MSTR
