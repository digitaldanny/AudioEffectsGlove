#include "build_switches.h"
#if ENABLE_UNIT_TEST_MPU6500_WHOAMI_SPI

#include "entry_points.h"
#include "mpu6500.h"

#define CLR_CS    GPIO_setOutputLowOnPin(systemIO.spiCs1.port, systemIO.spiCs1.pin)  // P3.5
#define SET_CS    GPIO_setOutputHighOnPin(systemIO.spiCs1.port, systemIO.spiCs1.pin)

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

int TEST_mpu6500WhoAmISpi()
{
    MAP_GPIO_setAsOutputPin(systemIO.spiCs1.port, systemIO.spiCs1.pin);

    //Drive the control lines to a reasonable starting state.
    SET_CS;

    uint8_t rx = 0;
    Spi::init();
    while (true)
    {
        // MPU6500_RA_WHO_AM_I
        CLR_CS;
        delayUs(2);

        rx = Spi::transferByte(0x80 | MPU6500_RA_WHO_AM_I); // READ bit | WHO_AM_I addr

        for (int i = 0; i < 5; i++)
        {
            rx = Spi::transferByte(0x12); // garbage to retreive data from rx buffer

            // if received data is non-zero, breakpoint to view
            if (rx > 0)
            {
                rx++;
            }
        }

        delayUs(2);
        SET_CS;

        delayMs(1);
    }
    return 0; // never gets here
}
#endif // ENABLE_UNIT_TEST_MPU6500_WHOAMI_SPI
