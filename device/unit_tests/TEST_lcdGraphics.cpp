#include "build_switches.h"
#if ENABLE_UNIT_TEST_LCD_TEXT

#include "entry_points.h"
#include "lcd_graphics.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: TEST_lcdGraphics
 * This test writes strings to all rows of the LCD. Below are the expected
 * outputs.
 *
 * Line 1: "Hello 0"
 * Line 2: "Hello 1"
 * Line 3: "Hello 2"
 * Line 4: "Hello 3"
 * Line 5: "Hello 4"
 * Line 6: "0123456789"
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int TEST_lcdGraphics()
{
    LcdGfx::init();
    LcdGfx::drawFill(0xFF); // Fill screen (shows full width of the LCD more clearly)
    LcdGfx::drawFill(0x00); // Clear screen
    LcdGfx::drawString(0, 0, "Hello 0", 7);
    LcdGfx::drawString(0, 1, "Hello 1", 7);
    LcdGfx::drawString(0, 2, "Hello 2", 7);
    LcdGfx::drawString(0, 3, "Hello 3", 7);
    LcdGfx::drawString(0, 4, "Hello 4", 7);
    LcdGfx::drawString(0, 5, "0123456789", 10);
    return 0;
}
#endif // ENABLE_UNIT_TEST_LCD_TEXT
