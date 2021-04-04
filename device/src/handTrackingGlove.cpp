#include "build_switches.h"
#if ENABLE_HAND_TRACKING_GLOVE

#include "entry_points.h"
#include "data_packet_protocol.h"
#include "hc05_api.h"
#include "flex_sensors_api.h"
#include "lcd_graphics.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+==5===+=====+=====+=====+=====+
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
    // Is master Hc-05 is paired with slave Hc-05?
    bool isSlaveConnected;

    // Is the slave Hc-05 ready to receive a new data packet?
    bool isSlaveReadyForUpdate;

    // Data packet to transfer
    dataPacket_t packet;

    // Message received back from the slave device
    // This will only contain the opcode portion of the dataPacket_t.
    char* slaveResponse;

    // Contains message to write to a single row of the LCD
    char lcdMsg[LCD_MAX_CHARS_PER_LINE];

} gloveState_t;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Contains information about the state of the main program
gloveState_t state;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * PROTOTYPES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

bool updateBluetoothConnectionStatus();
bool updateFlexSensorReadings();
void SendUpdateToSlave();
void WaitForSlaveAck();

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

    // Initialize GPIO to control external hardware power and set power to off
    initExternalHwPower();

    // Configures SPI module, LCD registers, and clears screen
    LcdGfx::init();

    // Configures ADC module and sets mux GPIO
    FlexSensors::Init();

    // Configure UART to communicate with HC-05 module
    Hc05Api::SetMode(HC05MODE_DATA);

    // Slave will be ready for update as soon as master and slave connect
    state.isSlaveReadyForUpdate = true;

    // Main program loop
    while(1)
    {
        // Is slave HC-05 still paired with master HC-05?
        updateBluetoothConnectionStatus();

        // Capture and compress ADC readings for all flex sensors
        updateFlexSensorReadings();

        // Send the next packet to the slave device if ready
        if (state.isSlaveConnected)
        {
            if (state.isSlaveReadyForUpdate)
            {
                SendUpdateToSlave();
            }
            else
            {
                // Wait for ACK response from slave device if packet was
                // recently sent.
                WaitForSlaveAck();
            }
        }

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
 *
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
bool updateBluetoothConnectionStatus()
{
    // Check if the master HC-05 module has paired with the
    // slave HC-05 module on the DSP Effects Rack C2000 board.
    state.isSlaveConnected = Hc05Api::IsSlaveConnected();

    if (state.isSlaveConnected)
    {
        memcpy(state.lcdMsg, "BT: Paired", LCD_MAX_CHARS_PER_LINE);
    }
    else
    {
        memcpy(state.lcdMsg, "BT: ?     ", LCD_MAX_CHARS_PER_LINE);
    }

    LcdGfx::drawString(LCD_COL_BLUETOOTH_STATUS, LCD_ROW_BLUETOOTH_STATUS,
                       state.lcdMsg, LCD_MAX_CHARS_PER_LINE);

    return true;
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: updateFlexSensorReadings
 * Read all compressed flex sensors voltage values (currently 3).
 *
 * RETURN: Always true
 *
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
bool updateFlexSensorReadings()
{
    for (uint8_t f = 0; f < FLEX_MAX_NUM_FINGERS; f++)
    {
        state.packet.flexSensors[f] = (unsigned char)FlexSensors::GetJointsData(f);

        // Update LCD with flex sensor readings
        memset(state.lcdMsg, 0, LCD_MAX_CHARS_PER_LINE);
        sprintf(state.lcdMsg, "F%u: %u", f, state.packet.flexSensors[f]);
        LcdGfx::drawString(0, f + 1, state.lcdMsg, LCD_MAX_CHARS_PER_LINE);
    }
    return true;
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: SendUpdateToSlave
 * Send latest packet of sensor data to the slave device.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
void SendUpdateToSlave()
{
    state.packet.opCode = DPP_OPCODE_PACKET;
    // Send next packet of data
    if (!Hc05Api::Send((char*) (&state.packet), sizeof(dataPacket_t)))
    {
        while (1); // Send transfer should not fail - trap CPU for debugging.

    }
    // Slave may still be processing the previous update.
    state.isSlaveReadyForUpdate = false;
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: WaitForSlaveAck
 * Loop infinitely while waiting for slave device to notify master that
 * previous data packet was processed.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
void WaitForSlaveAck()
{
    // Wait for ACK response from slave device
    if (!Hc05Api::Recv((char**) (&state.slaveResponse), 1))
    {
        while (1); // Receive should not fail - trap CPU for debugging.

    }
    if (*state.slaveResponse != DPP_OPCODE_ACK)
    {
        while (1); // Slave response should only be an ACK - trap CPU for debugging.

    }
    // Slave has processed the previous update and is ready for a new one.
    state.isSlaveReadyForUpdate = true;
}

#endif // ENABLE_HAND_TRACKING_GLOVE
