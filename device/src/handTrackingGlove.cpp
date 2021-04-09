#include "build_switches.h"
#if ENABLE_HAND_TRACKING_GLOVE

#include "entry_points.h"
#include "data_packet_protocol.h"
#include "hc05_api.h"
#include "flex_sensors_api.h"
#include "mpu6050_api.h"
#include "sensor_processing_lib.h"
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

    // MPU6050 sensor data
    int16_t accelBuffer[3];
    int16_t gyroBuffer[3];

    // Sensor fusion variables
    unsigned long startTime;
    float delta,wx,wy,wz;
    euler_angles angles;
    vector_ijk fusedVector;
    Quaternion qAcc;
    float pitch;
    float roll;

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
void updatePitchRoll();
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

    // Configure I2C and MPU6050 module to read accelerometer and gyroscope sensor data.
    Mpu6050Api::init();

    // Initialize sensor fusion library
    state.fusedVector = vector_3d_initialize(0.0,0.0,-1.0);
    state.qAcc = quaternion_initialize(1.0,0.0,0.0,0.0);
    state.startTime = millis();

    // Slave will be ready for update as soon as master and slave connect
    state.isSlaveReadyForUpdate = true;

    // Main program loop
    while(1)
    {
        // Is slave HC-05 still paired with master HC-05?
        updateBluetoothConnectionStatus();

        // Capture and compress ADC readings for all flex sensors
        updateFlexSensorReadings();

        // Capture the latest accelerometer and gyroscope sensor readings
        Mpu6050Api::readSensorData(state.accelBuffer, state.gyroBuffer);

        // Update pitch and roll angles from accel / gyro readings.
        updatePitchRoll();

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
 * Description: updateEulerAngles
 * Use the sensor fusion algorithm to convert most recent gyroscope and
 * accelerometer readings into new Pitch and Roll values.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
void updatePitchRoll()
{
    // Update Euler angles from accel / gyro readings
    state.wx = 0.0005323 * state.gyroBuffer[0];
    state.wy = 0.0005323 * state.gyroBuffer[1];
    state.wz = 0.0005323 * state.gyroBuffer[2];

    state.delta = 0.001 * (millis() - state.startTime);

    state.fusedVector = update_fused_vector(state.fusedVector, state.accelBuffer[0],
                                            state.accelBuffer[1], state.accelBuffer[2],
                                            state.wx, state.wy, state.wz,
                                            state.delta);

    state.qAcc = quaternion_from_accelerometer(state.fusedVector.a,
                                               state.fusedVector.b,
                                               state.fusedVector.c);

    state.angles = quaternion_to_euler_angles(state.qAcc);
    state.startTime = millis();

    // Modify the roll values so that the resting sensor has an angle of 0.
    // Rotating right increases towards 180, rotating left decreases towards -180.
    state.roll = state.angles.roll + 180.0f;
    if (state.roll > 180.0f)
    {
        state.roll -= 360.0f;
    }

    // TODO - Modify the pitch values so that there is not any bending at the
    // top of 90 degrees and -90 degrees.
    state.pitch= state.angles.pitch;

    // Serialize the pitch and roll
    sprintf((char*)state.packet.pitch, "%*.*f",DPP_FLOAT_LENGTH_TOTAL, DPP_FLOAT_LENGTH_DECIMAL, state.pitch);
    sprintf((char*)state.packet.roll, "%*.*f",DPP_FLOAT_LENGTH_TOTAL, DPP_FLOAT_LENGTH_DECIMAL, state.roll);

    // Update LCD with Pitch and Roll angles.
    LcdGfx::drawString(0, 1, (char*)state.packet.pitch, DPP_FLOAT_LENGTH_TOTAL);
    LcdGfx::drawString(0, 2, (char*)state.packet.roll, DPP_FLOAT_LENGTH_TOTAL);
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
