#include "build_switches.h"
#if ENABLE_UNIT_TEST_STATE_OF_CHARGE

#include "entry_points.h"
#include "adc_if.h"
#include "lcd_graphics.h"
#include "flex_sensors_api.h"

#define ADC_CH_OCV      ADC_CH3 // Open circuit voltage ADC buffer index
#define ADC_CH_CCV      ADC_CH1 // Closed circuit voltage ADC buffer index

uint16_t flex0;
uint16_t ocv;
uint16_t ccv;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * Description: unitTest_hc05RwToSlave
 * This captures an initial state of charge estimation by comparing the battery's
 * Open Circuit Voltage with a LUT containing samples from the battery's discharge
 * curve. After that, it continuously estimates the glove hardware's current
 * draw while the load is attached.
 *
 * NOTES:
 * This test also utilizes the Pch MOSFET power switch, so the external power
 * GPIO connection defined in the systemIO variable must be connected.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
int unitTest_stateOfCharge()
{
    char lcdBuff[10] = {0};

    Adc::Init();
    LcdGfx::init();

    ocv = Adc::ReadAdcChannel(ADC_CH_OCV);
    memset(lcdBuff, ' ', 10);
    sprintf(lcdBuff, "OCV: %u", ocv);
    LcdGfx::drawString(0, 0, lcdBuff, 10);

    while(true)
    {
        ccv = Adc::ReadAdcChannel(ADC_CH_CCV);
        memset(lcdBuff, ' ', 10);
        sprintf(lcdBuff, "CCV: %u", ccv);
        LcdGfx::drawString(0, 1, lcdBuff, 10);

        delayMs(10);
    }
    return 0;
}

#endif // ENABLE_UNIT_TEST_STATE_OF_CHARGE
