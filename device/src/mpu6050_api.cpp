/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "mpu6050_api.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * GLOBALS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Stores values from MPU6050 accelerometer, temperature, and gyroscope registers.
uint8_t sensorBuffer[MPU6050_SENSOR_BURST_LEN];

// NOTE - Only append to the end of this array. **ORDER MATTERS**
// Each config contains the following - Register addr, register value
uint8_t mpu6050Configs[MPU6050_NUM_CONFIGS][2] = {
    /* MPU6050_PWR_MGMT_1   */ {0x6B, 0x00}, // Wake up device
    /* MPU6050_GYRO_CONFIG  */ {0x1B, 0x08}, // +- 500 deg/s
    /* MPU6050_ACCEL_CONFIG */ {0x1C, 0x08}  // +- 4g
};

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * FUNCTIONS
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
bool Mpu6050Api::init()
{
    // Initialize I2C module and configure MPU6050 registers
    I2c::init();

    // Write configuration register values
    for (int iConfig = 0; iConfig < MPU6050_NUM_CONFIGS; iConfig++)
    {
        if (!I2c::write(MPU6050_DEV_ADDR, mpu6050Configs[iConfig][0], 1, &mpu6050Configs[iConfig][1]))
        {
            return false;
        }
        delayMs(1);
    }

    delayMs(10);

    return true;
}

bool Mpu6050Api::readSensorData(int16_t* accelBuffer, int16_t* gyroBuffer)
{
    if (!I2c::read(MPU6050_DEV_ADDR, MPU6050_SENSOR_ADDR_START, MPU6050_SENSOR_BURST_LEN, sensorBuffer))
    {
        return false;
    }

    accelBuffer[0] = (sensorBuffer[0] << 8) | (sensorBuffer[1]);
    accelBuffer[1] = (sensorBuffer[2] << 8) | (sensorBuffer[3]);
    accelBuffer[2] = (sensorBuffer[4] << 8) | (sensorBuffer[5]);
    // temperature value is stored in indices 6-7 but is not used for this application
    gyroBuffer[0] = ((sensorBuffer[8] << 8) | (sensorBuffer[9])) - MPU6050_GYRO_ERROR_X;
    gyroBuffer[1] = ((sensorBuffer[10] << 8) | (sensorBuffer[11])) - MPU6050_GYRO_ERROR_Y;
    gyroBuffer[2] = ((sensorBuffer[12] << 8) | (sensorBuffer[13])) - MPU6050_GYRO_ERROR_Z;

    return true;
}
