#include "build_switches.h"

#if ENABLE_UNIT_TEST_EXT_PWR_SWITCH

#include "unit_tests.h"
#include "target_hw_common.h"

// Enable test that toggles external power switch. Power output be manually validated with DAD board.
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
