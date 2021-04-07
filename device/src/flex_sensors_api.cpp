#include "flex_sensors_api.h"

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Init
 * Initialize hardware for collecting joint data.
 * 
 * INPUTS: N/A
 * 
 * RETURN:
 * bool - True if hardware setup was successful.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool FlexSensors::Init()
{
    if (!Mux::Init())
    {
        // Issue initializing Mux, trap CPU
        while(1);
    }
    if (!Adc::Init())
    {
        // Issue initializing ADC, trap CPU
        while(1);
    }
    return true;
}

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: GetJointsData
 * Get MCP and PIP joint data for the selected finger.
 * 
 * INPUTS:
 * @finger
 *      Which finger to collect data for (thumb=0, pointer=1, middle=2).
 * 
 * RETURN:
 * uint8_t - Requested finger's ADC reading compressed down to an 8-bit value.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
uint8_t FlexSensors::GetJointsData(uint8_t finger)
{
    uint16_t adcReadingRaw;

    // Check that valid finger index is being requested
    if (finger >= FLEX_MAX_NUM_FINGERS)
    {
        while(1); // Request exceeded max number of flex sensors - trap CPU
    }

    // Select which flex sensor voltage to output on the MCP and PIP muxes.
    if (!Mux::SelectMuxChannel(finger))
    {
        return false;
    }

    delayMs(1);

    // Collect ADC sample from the flex sensor amplifier output
    adcReadingRaw = Adc::ReadAdcChannel(ADC_CHAN_FLEX_SENSORS);

    // Compression - Max value read during testing was about 8000.
    // This means the top bit is not required, so I am interested
    // in bits B12:B5. Therefore, I shift the reading to the right
    // by 5 to get the most useful readings.
    return (uint8_t)(adcReadingRaw>>6);
}
