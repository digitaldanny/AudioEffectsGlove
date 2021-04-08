#include "build_switches.h"
#if ENABLE_UNIT_TEST_STATE_OF_CHARGE

#include "entry_points.h"
#include "adc_if.h"
#include "lcd_graphics.h"
#include "flex_sensors_api.h"

#define ADC_CH_OCV      ADC_CH2 // Open circuit voltage ADC buffer index
#define ADC_CH_CCV      ADC_CH3 // Closed circuit voltage ADC buffer index

uint8_t flex0;
uint8_t ocv;
uint8_t ccv;

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
    FlexSensors::Init();
    //LcdGfx::init();
    FlexSensors::GetJointsData(0);

    while(true)
    {
        flex0   = (uint8_t)(Adc::ReadAdcChannel(0) >> 6);
        ocv     = (uint8_t)(Adc::ReadAdcChannel(ADC_CH_OCV) >> 6);
        ccv     = (uint8_t)(Adc::ReadAdcChannel(ADC_CH_CCV));

        //memset(lcdBuff, ' ', 10);
        //sprintf(lcdBuff, "FLX: %u", flex0);
        //LcdGfx::drawString(0, 0, lcdBuff, 10);
        //
        //memset(lcdBuff, ' ', 10);
        //sprintf(lcdBuff, "OCV: %u", ocv);
        //LcdGfx::drawString(0, 1, lcdBuff, 10);
        //
        //memset(lcdBuff, ' ', 10);
        //sprintf(lcdBuff, "CCV: %u", ccv);
        //LcdGfx::drawString(0, 2, lcdBuff, 10);

        delayMs(100);
    }
    return 0;
}

#endif // ENABLE_UNIT_TEST_STATE_OF_CHARGE
