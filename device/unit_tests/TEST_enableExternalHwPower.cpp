#include "build_switches.h"

#if ENABLE_UNIT_TEST_EXT_PWR_SWITCH

#include "entry_points.h"
#include "target_hw_common.h"

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: unitTest_enableExternalHwPower
 * This test toggles the external power switch. Power output must be manually validated
 * with DAD board.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
void unitTest_enableExternalHwPower()
{
    initExternalHwPower();

    while (true)
    {
        // enable power switch
        setExternalHwPower(true);
        delayMs(1);

        // disable power switch
        setExternalHwPower(false);
         delayMs(1);
    }
}
#endif // ENABLE_UNIT_TEST_EXT_PWR_SWITCH
