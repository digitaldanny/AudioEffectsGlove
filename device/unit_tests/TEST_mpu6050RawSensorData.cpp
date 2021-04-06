#include "build_switches.h"
#if ENABLE_UNIT_TEST_MPU6050_SENSORDATA_RAW

#include "entry_points.h"
#include "i2c_if.h"

// Configuration register indices
#define MPU6050_NUM_CONFIGS         3
#define MPU6050_PWR_MGMT_1          0
#define MPU6050_GYRO_CONFIG         1
#define MPU6050_ACCEL_CONFIG        2

// Other
#define MPU6050_DEV_ADDR            0x68
#define MPU6050_SENSOR_ADDR_START   0x3B    // XACCEL_H is the first address to read from to capture all sensor values
#define MPU6050_SENSOR_BURST_LEN    14      // 6 regs Accel X-Z H/L, 2 regs Temp H/L, 6 regs Gyro X-Z H/L

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
 * Description: TEST_mpu6500RawSensorData
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int TEST_mpu6500RawSensorData()
{
    I2c::init();

    // Write configuration register values
    for (int iConfig = 0; iConfig < MPU6050_NUM_CONFIGS; iConfig++)
    {
        if (!I2c::write(MPU6050_DEV_ADDR, mpu6050Configs[iConfig][0], 1, &mpu6050Configs[iConfig][1]))
        {
            while(1); // I2c NACK received - track CPU for HW debug
        }
        delayMs(10);
    }

    delayMs(10);

    // Read accelerometer and gyro raw values
    while(1)
    {
        // Update sensor buffer with new readings
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

        delayMs(10); // delay here for breakpointing
    }
    return 0;
}
#endif // ENABLE_UNIT_TEST_MPU6050_WHOAMI_I2C
