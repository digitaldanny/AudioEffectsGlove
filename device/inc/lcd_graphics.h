#ifndef _SRC_LCD_GRAPHICS
#define _SRC_LCD_GRAPHICS

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 */

#include "build_switches.h"
#include "target_hw_common.h"
#include "lcd_64x48_bitmap.h"

namespace LcdGfx {
    bool init();
    bool writeDataBlock(unsigned char* data, uint16_t len);
    bool drawString(uint8_t x, uint8_t y, char* msg, uint16_t msgLen);
    bool drawFill(unsigned char pattern);
}

#endif // _SRC_LCD_GRAPHICS
