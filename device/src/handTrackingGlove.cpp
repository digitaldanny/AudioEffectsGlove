#include "build_switches.h"
#if ENABLE_HAND_TRACKING_GLOVE

#include "entry_points.h"
#include "hc05_api.h"
#include "lcd_64x48_bitmap.h"

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
    return 0;
}

#endif // ENABLE_HAND_TRACKING_GLOVE
