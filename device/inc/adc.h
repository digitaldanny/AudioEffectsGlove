#ifndef _SRC_ADC
#define _ADC

// Common target includes
#include "build_switches.h"
#include <stdio.h>

#if ENABLE_ADC_PYTHON
#include "python_common.h"

#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif // ENABLE_WIRELESS_API_PYTHON

#if TARGET_HW_DESKTOP
#include "python3.6/Python.h"
#endif // TARGET_HW_DESKTOP
#endif // ENABLE_WIRELESS_API_PYTHON

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* DEFINES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#define ADC_MODULE          PROTO_PATH "mcp3008_adc"
#define ADC_CLASS           "Mcp3008"
#define ADC_INIT_FUNC       "__enter__"
#define ADC_READ_FUNC       "readAdc"

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