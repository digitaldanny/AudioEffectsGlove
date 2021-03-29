#include "build_switches.h"
#if ENABLE_UNIT_TEST_LCD_DEMO

#include "unit_tests.h"
#include "lcd_64x48_bitmap.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: TEST_lcdProductDemo
 * This test runs the product demo for the CFAL6448A LCD provided by the
 * developers.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int TEST_lcdProductDemo()
{
    lcd_setup();
    while(1)
    {
        lcd_loop();
    }
    return 0;
}
#endif // ENABLE_UNIT_TEST_LCD_DEMO
