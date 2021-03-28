#include "build_switches.h"
#if ENABLE_UNIT_TEST_MPU6050_WHOAMI_I2C

#include "unit_tests.h"
#include "i2c_if.h"

#define MPU6050_DEVADDR     0x68
#define WHO_AM_I_REGADDR    0x75
#define WHO_AM_I_VALUE      0x68

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: TEST_mpu6500WhoAmISpi
 * This test expects the user to have the MSP432 connected to the MPU6500 module
 * according to the SPI pin connections defined in the systemIO variable.
 *
 * This test reads back the MPU6500 WHO_AM_I register to confirm that the device
 * is functional and hardware connections are correct.
 *
 * This test also runs infinitely to allow for triggering on a logic analyzer.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int TEST_mpu6500WhoAmII2c()
{
    uint8_t data;

    I2c::init();

    while(1)
    {
        I2c::read(MPU6050_DEVADDR, WHO_AM_I_REGADDR, 1, &data);
    }

    if (data == WHO_AM_I_VALUE)
    {
        // Test passes!
        return 0;
    }
    else
    {
        return -1;
    }
}
#endif // ENABLE_UNIT_TEST_MPU6050_WHOAMI_I2C
