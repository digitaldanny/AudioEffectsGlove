#ifndef _SRC_FLEX_SENSORS_API
#define _SRC_FLEX_SENSORS_API

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* INCLUDES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "target_hw_common.h"
#include <adc_if.h>
#include <mux_if.h>

#define ADC_CHAN_MCP    (0) // ADC channel used to read MCP joint data
#define ADC_CHAN_PIP    (1) // ADC channel used to read PIP joint data

#define ADC_CHAN_FLEX_SENSORS   ADC_CH0 // ADC channel being used to read flex sensor values

#define FLEX_MAX_NUM_FINGERS    3

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* STRUCTS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

namespace FlexSensors {
    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    
    bool Init();
    uint8_t GetJointsData(uint8_t finger);
}

#endif // _SRC_FLEX_SENSORS_API
