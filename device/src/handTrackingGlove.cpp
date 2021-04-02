#include "build_switches.h"
#if ENABLE_HAND_TRACKING_GLOVE

#include "entry_points.h"
#include "hc05_api.h"
#include "lcd_graphics.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#define LCD_ROW_BLUETOOTH_STATUS    0
#define LCD_COL_BLUETOOTH_STATUS    0

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * STRUCTS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

typedef struct
{
    bool isSlaveConnected;
} gloveState_t;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Contains information about the state of the main program
gloveState_t state;

// Contains message to write to a single row of the LCD
char lcdMsg[LCD_MAX_CHARS_PER_LINE];

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * PROTOTYPES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

bool updateBluetoothConnectionStatus();

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: handTrackingGlove
 * This is the main program loop of the hand tracking glove. The below bullet
 * points show the goals of this process.
 *
 * Init
 * - Capture Open Circuit Voltage of the battery to estimate an initial
 *   State of Charge.
 *
 * Loop
 * - Capture ADC readings for each flex sensor on the glove (currently 3 sensors).
 * - Capture accelerometer and gyroscope readings. Use a sensor fusion algorithm
 *   to convert these raw values into euler angles for determining hand orientation.
 * - Capture current draw and update the battery's State of Charge estimation.
 * - Check that the master HC-05 module is still connected to the slave HC-05 module.
 * - Send updated sensor data to the slave HC-05 module (DSP Effects Rack).
 * - Update the LCD with Bluetooth connection status, current draw, and battery charge.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int handTrackingGlove()
{
    // Initialize glove state variable
    memset((void*)&state, 0, sizeof(gloveState_t));

    // Configures SPI module, LCD registers, and clears screen
    LcdGfx::init();

    // Main program loop
    while(1)
    {
        updateBluetoothConnectionStatus();
        delayMs(1);
    }

    return 0;
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: updateBluetoothConnectionStatus
 * Use the HC-05 STATE pin to check if the master HC-05 is paired with
 * the slave HC-05 on the DSP Effects Rack board.
 *
 * RETURN: Always true
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
bool updateBluetoothConnectionStatus()
{
    // Check if the master HC-05 module has paired with the
    // slave HC-05 module on the DSP Effects Rack C2000 board.
    state.isSlaveConnected = Hc05Api::IsSlaveConnected();

    if (state.isSlaveConnected)
    {
        memcpy(lcdMsg, "BT: Paired", LCD_MAX_CHARS_PER_LINE);
    }
    else
    {
        memcpy(lcdMsg, "BT: ?     ", LCD_MAX_CHARS_PER_LINE);
    }

    LcdGfx::drawString(LCD_COL_BLUETOOTH_STATUS, LCD_ROW_BLUETOOTH_STATUS,
                       lcdMsg, LCD_MAX_CHARS_PER_LINE);

    return true;
}

#endif // ENABLE_HAND_TRACKING_GLOVE
