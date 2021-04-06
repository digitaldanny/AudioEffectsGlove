#include "build_switches.h"
#if ENABLE_HAND_TRACKING_GLOVE

#include "entry_points.h"
#include "data_packet_protocol.h"
#include "hc05_api.h"
#include "flex_sensors_api.h"
#include "i2c_if.h"
#include "lcd_graphics.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+==5===+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#define LCD_ROW_BLUETOOTH_STATUS    0
#define LCD_COL_BLUETOOTH_STATUS    0

#define BLUETOOTH_TIMEOUT_COUNT     500 // Iteration count between data update to ack is expected to be around 10-15.

/*
 * +=====+=====+=====+=====+=====+=====+=====+==5===+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Configuration register indices
#define MPU6050_NUM_CONFIGS         3
#define MPU6050_PWR_MGMT_1          0
#define MPU6050_GYRO_CONFIG         1
#define MPU6050_ACCEL_CONFIG        2

// Other
#define MPU6050_DEV_ADDR            0x68
#define MPU6050_SENSOR_ADDR_START   0x3B    // XACCEL_H is the first address to read from to capture all sensor values
#define MPU6050_SENSOR_BURST_LEN    14      // 6 regs Accel X-Z H/L, 2 regs Temp H/L, 6 regs Gyro X-Z H/L

/*
 * +=====+=====+=====+=====+=====+=====+=====+==5===+=====+=====+=====+=====+
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

uint8_t sensorBuffer[MPU6050_SENSOR_BURST_LEN];
int16_t accelBuffer[3];
int16_t gyroBuffer[3];

// NOTE - Only append to the end of this array. **ORDER MATTERS**
// Each config contains the following - Register addr, register value
uint8_t mpu6050Configs[MPU6050_NUM_CONFIGS][2] = {
    /* MPU6050_PWR_MGMT_1   */ {0x68, 0x00},
    /* MPU6050_GYRO_CONFIG  */ {0x1B, 0x08}, // +- 500 deg/s
    /* MPU6050_ACCEL_CONFIG */ {0x1C, 0x08} // +- 4g
};

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

    // If slave device has not responded within a certain number of
    // main loop iterations, the C2000 program may have restarted
    // without powering off the slave HC-05.
    uint32_t bluetoothTimeoutCounter;

    // Data packet to transfer
    dataPacket_t packet;

    // Message received back from the slave device
    // This will only contain the opcode portion of the dataPacket_t.
    char* slaveResponse;

    // Contains message to write to a single row of the LCD
    char lcdMsg[LCD_MAX_CHARS_PER_LINE];

} gloveState_t;

/*
 * +=====+=====+=====+=====+=====+=====+=====+
 * =====+=====+=====+=====+=====+
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
void CheckForSlaveAck();

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

    // Initialize I2C module and configure MPU6050 registers
    I2c::init();

    // Write configuration register values
    for (int iConfig = 0; iConfig < MPU6050_NUM_CONFIGS; iConfig++)
    {
        I2c::write(MPU6050_DEV_ADDR, mpu6050Configs[iConfig][0], 1, &mpu6050Configs[iConfig][1]);
        delayMs(1);
    }

    delayMs(10);

    // Main program loop
    while(1)
    {
        // Is slave HC-05 still paired with master HC-05?
        updateBluetoothConnectionStatus();

        // Capture and compress ADC readings for all flex sensors
        updateFlexSensorReadings();

        /////// UDPATE ACCELEROMETER AND GYROSCOPE DATA HERE
        if (!I2c::read(MPU6050_DEV_ADDR, MPU6050_SENSOR_ADDR_START, MPU6050_SENSOR_BURST_LEN, sensorBuffer))
        {
            while (1); // read failed, trap CPU for debugging
        }

        accelBuffer[0] = (sensorBuffer[0] << 8) | (sensorBuffer[1]);
        accelBuffer[1] = (sensorBuffer[2] << 8) | (sensorBuffer[3]);
        accelBuffer[2] = (sensorBuffer[4] << 8) | (sensorBuffer[5]);
        // temp value is stored in indices 6-7
        gyroBuffer[0] = (sensorBuffer[8] << 8) | (sensorBuffer[9]);
        gyroBuffer[1] = (sensorBuffer[10] << 8) | (sensorBuffer[11]);
        gyroBuffer[2] = (sensorBuffer[12] << 8) | (sensorBuffer[13]);

        for (uint8_t f = 0; f < 3; f++)
        {
            memset(state.lcdMsg, 0, LCD_MAX_CHARS_PER_LINE);
            sprintf(state.lcdMsg, "A%u: %u", f, accelBuffer[f]);
            LcdGfx::drawString(0, f + 1, state.lcdMsg, LCD_MAX_CHARS_PER_LINE);
        }
        /////// UDPATE ACCELEROMETER AND GYROSCOPE DATA HERE

        if (state.isSlaveConnected)
        {
            // Send the next packet to the slave device if ready
            if (state.isSlaveReadyForUpdate)
            {
                SendUpdateToSlave();
            }
            else if (state.bluetoothTimeoutCounter >= BLUETOOTH_TIMEOUT_COUNT)
            {
                // Slave device may have disconnected. Reset slaveReady bool
                // for when the device reconnects.
                state.isSlaveReadyForUpdate = true;
            }
            else
            {
                // Wait for ACK response from slave device if packet was
                // sent during current connection.
                CheckForSlaveAck();
            }
        }

        state.bluetoothTimeoutCounter++;
        delayMs(5);
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

        // // Update LCD with flex sensor readings
        // memset(state.lcdMsg, 0, LCD_MAX_CHARS_PER_LINE);
        // sprintf(state.lcdMsg, "F%u: %u", f, state.packet.flexSensors[f]);
        // LcdGfx::drawString(0, f + 1, state.lcdMsg, LCD_MAX_CHARS_PER_LINE);
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
    state.bluetoothTimeoutCounter = 0;
}

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: WaitForSlaveAck
 * Loop infinitely while waiting for slave device to notify master that
 * previous data packet was processed.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
void CheckForSlaveAck()
{
    // Wait for ACK response from slave device
    if (Hc05Api::Recv((char**) (&state.slaveResponse), 1))
    {
        // Decode response from slave device
        if (*state.slaveResponse != DPP_OPCODE_ACK)
        {
            while (1); // Slave response should only be an ACK - trap CPU for debugging.
        }

        // Slave has processed the previous update and is ready for a new one.
        state.isSlaveReadyForUpdate = true;
    }
}

#endif // ENABLE_HAND_TRACKING_GLOVE
