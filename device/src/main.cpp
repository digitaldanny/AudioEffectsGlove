#include <stdio.h>

#include "build_switches.h"
#include "target_hw_common.h"
#include "unit_tests.h"

/// DEBUG **********
#include "lcd_64x48_bitmap.h"
#include "i2c_if.h"
#include "spi_if.h"
/// DEBUG **********

int main()
{
    setupTargetHw(); // MSP432 configurations (clock speed, WDT disable, etc)
    initExternalHwPower(); // Initialize external hardware power (off by default)

#if ENABLE_UNIT_TEST_LCD_DEMO

    setExternalHwPower(true);
    lcd_setup();
    //while(true)
    //{
    //    lcd_test();
    //}
    lcd_loop();
#endif // ENABLE_UNIT_TEST_LCD_DEMO

#if ENABLE_UNIT_TEST_I2C
    I2c::init();
    while (true)
    {
        uint8_t tx[3] = {0x12, 0x34, 0x56};
        //uint8_t rx[3] = {0};
        I2c::write(0xAB, 0xCD, 3, tx);
        //I2c::read(0xAB, 0xCD, 3, tx);
        delayMs(1);
    }
#endif // ENABLE_UNIT_TEST_I2C

#if ENABLE_UNIT_TEST_MPU6500_WHOAMI_SPI
#define CLR_CS    GPIO_setOutputLowOnPin(systemIO.spiCs1.port, systemIO.spiCs1.pin)  // P3.5
#define SET_CS    GPIO_setOutputHighOnPin(systemIO.spiCs1.port, systemIO.spiCs1.pin)

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
#endif // ENABLE_UNIT_TEST_MPU6500_WHOAMI_SPI

#if ENABLE_UNIT_TEST_MPU6500_WHOAMI_I2C
    I2c::init();
    mpu6500Init();
    while(true)
    {
        mpu6500TestConnection();
        delayMs(1);
    }
#endif // ENABLE_UNIT_TEST_MPU6500_WHOAMI_I2C

#if ENABLE_UNIT_TEST_EXT_PWR_SWITCH
    unitTest_enableExternalHwPower();
#endif // ENABLE_UNIT_TEST_EXT_PWR_SWITCH

#if ENABLE_UNIT_TEST_WIRELESS_API
    unitTest_wirelessApi();
#endif // ENABLE_UNIT_TEST_WIRELESS_API

#if ENABLE_UNIT_TEST_NXP_API
    unitTest_nxpApi();
#endif // ENABLE_UNIT_TEST_NXP_API

#if ENABLE_UNIT_TEST_MUX
    unitTest_mux();
#endif // ENABLE_UNIT_TEST_MUX

#if ENABLE_UNIT_TEST_MUX_LP
    unitTest_muxLp();
#endif // ENABLE_UNIT_TEST_MUX_C2000

#if ENABLE_UNIT_TEST_ADC
    unitTest_adc();
#endif // ENABLE_UNIT_TEST_ADC

#if ENABLE_UNIT_TEST_ADC_C2000
    unitTest_adcC2000();
#endif // ENABLE_UNIT_TEST_ADC_C2000

#if ENABLE_UNIT_TEST_ADC_MSP432
    unitTest_adcMsp432();
#endif // ENABLE_UNIT_TEST_ADC_MSP432

#if ENABLE_UNIT_TEST_FLEX_SENSORS
    unitTest_flexSensors();
#endif // ENABLE_UNIT_TEST_FLEX_SENSORS

#if ENABLE_UNIT_TEST_HC05_DEVICE_NAME
    unitTest_hc05DeviceName();
#endif // ENABLE_UNIT_TEST_HC05_DEVICE_NAME

#if ENABLE_MAIN_V1
    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
#endif // ENABLE_MAIN_V1
}
