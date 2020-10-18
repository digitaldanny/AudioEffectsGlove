#ifndef _SRC_ADC
#define _ADC

// Common target includes
#include "build_switches.h"
#include <stdio.h>

#if ENABLE_ADC_PYTHON
#include "python_common.h"

#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif // TARGET_HW_RASPBERRYPI

#if TARGET_HW_DESKTOP
#include "python3.6/Python.h"
#endif // TARGET_HW_DESKTOP
#endif // ENABLE_ADC_PYTHON

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* DEFINES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Python module defines
#define ADC_MODULE          PROTO_PATH "mcp3008_adc"
#define ADC_CLASS           "Mcp3008"
#define ADC_INIT_FUNC       "__enter__"
#define ADC_READ_FUNC       "readAdc"

// ADC channel defines
#define ADC_CHAN_MCP        0
#define ADC_CHAN_PIP        1

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PROTOTYPES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

namespace Adc {

    bool Init               ();
    int ReadAdcChannel      (int adc_channel);

#if ENABLE_ADC_PYTHON
    namespace Python {

        bool Init               ();
        int ReadAdcChannel      (int adc_channel);
        
    }
#endif
}

#endif