#include "build_switches.h"
#if ENABLE_HC05_CONFIG_MSTR

#include "entry_points.h"
#include "hc05_api.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_hc05DeviceName
 * This test configures the master HC-05's role and slave address.
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
    return -1;
}

#endif // ENABLE_HC05_CONFIG_MSTR
