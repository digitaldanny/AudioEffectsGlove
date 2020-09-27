#include "flex_sensors_api.h"

bool FlexSensors::Init()
{
    if (!Mux::Init() || !Adc::Init())
    {
        printf("FlexSensors::Init - Could not initialize Mux or ADC\n");
        return false;
    }
    return true;
}

bool FlexSensors::GetJointsData(FlexSensors::finger_e finger, FlexSensors::fingerJoints_t* joints)
{
    int mcpAdcReading;
    int pipAdcReading;

    // Select which flex sensor voltage to output on the MCP and PIP muxes.
    if (!Mux::SelectMuxChannel((int)finger))
    {
        printf("FlexSensors::GetJointsData - Could not set the mux channel.\n");
        return false;
    }

    // Read from both ADC channels to collect an ADC sample for the mux outputs
    // ADC0 - MCP mux output
    // ADC1 - PIP mux output
    if ((mcpAdcReading = Adc::ReadAdcChannel(ADC_CHAN_MCP)) == -1)
    {
        printf("FlexSensors::GetJointsData - Could not get MCP ADC reading.\n");
        return false;
    }
    if ((pipAdcReading = Adc::ReadAdcChannel(ADC_CHAN_PIP)) == -1)
    {
        printf("FlexSensors::GetJointsData - Could not get MCP ADC reading.\n");
        return false;
    }

    // Assign the ADC readings to the passed fingerJoints structure.
    joints->mcp = mcpAdcReading;
    joints->pip = pipAdcReading;
    return true;
}