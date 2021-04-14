/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "state_of_charge_api.h"

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: getChargeFromOcv
 * Set up "non-module" hardware registers.
 * Ex of included inits.) Set clock speed to 48 MHz, disable WDT
 * Ex of inits NOT included.) Configuring UART to transmit data over Bluetooth
 *
 * See link below for plot that this estimation is based on.
 * https://github.com/digitaldanny/GestureControl/issues/51
 *
 * INPUTS:
 * @ocv -
 *
 * RETURN:
 * float - Estimated state of charge based on the battery's discharge plot.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
float SocApi::getChargeFromOcv(uint16_t ocv)
{
    float soc;

    float ocvf = ADC_ERR*ADC_VREF*(float)ocv/(float)ADC_MAX_VALUE/OCV_VDIV;

    // In the linear portion of the SOC and OCV relationship?
    if (ocv > 3.5f)
    {
        soc = (100.0f - 20.0f) / (4.2f - 3.5f) * ocvf - 380.0f;
    }

    // In the logarithmic portion of the relationship?
    else
    {
        soc = 20.0f * log10(ocvf/3.5f); // soc is 20% at about 3.5 OCV
    }

    // Boundary checks
    if (soc < 0.0f)
        return 0.0f;
    else if (soc > 100.0f)
        return 100.0f;
    else
        return soc;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: calculateStateOfCharge
 * Calculate the new state of charge based on the previous state of charge and
 * the difference in time.
 *
 * INPUTS:
 * @prevSoc -
 *
 * RETURN:
 * float - New estimated state of charge.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
float SocApi::calculateStateOfCharge(float prevSoc, float currentLoad)
{
    static uint32_t prevMillis = 0;

    uint32_t currMillis = millis();

    float soc = prevSoc + currentLoad * (((float)currMillis - (float)prevMillis)/1000.0f * MS_PER_HOUR) / BATTERY_NOMINAL_CAPACITY_MAH;

    // Boundary checks
    if (soc < 0.0f)
        soc = 0.0f;
    else if (soc > 100.0f)
        soc = 100.0f;

    prevMillis = currMillis;

    return soc;
}
