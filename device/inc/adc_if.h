#ifndef _SRC_ADC
#define _SRC_ADC

// Common target includes
#include "build_switches.h"
#include "target_hw_common.h"
#include <stdio.h>

#if ENABLE_ADC_C2000
 #include "string.h"
#endif // ENABLE_ADC_C2000

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

// C2000 defines
#define ADC_NUM_READINGS_BEFORE_VALID   (2) // Number of ADC readings required before a value can be read

// ADC channel defines
#define ADC_A               (0)   ///< Adc module A
#define ADC_B               (1)   ///< Adc module B
#define ADC_MAX_NUM_CHAN    (2)   ///< Number of ADC channels used

// MSP432 channel
#define ADC_CH0             (0)
#define ADC_CH1             (1)
#define ADC_CH2             (2)
#define ADC_CH3             (3)

#define ADC_MAX_VALUE       16383   // 2^14-1
#define ADC_VREF            3.3f    // Voltage reference for ADC channel
#define OCV_VDIV            (100000.0f/(100000.0f + 100000.0f)) // Voltage divider formula for OCV voltage divider
#define ADC_ERR             1.26f   // Percentage error of expected value - should be 1.0 ideally

// Multi-sequence ADC conversion being used - 4 ADC_MEM regions.
#define NUM_ADC_CONVERSIONS_MAX 4

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* STRUCTS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#if ENABLE_ADC_C2000
/*
 * DESCRIPTION: adcData_t
 * @value                   uint16      12-bit ADC reading.
 * @capturesSinceLastRead   uint32_t    Number of new sample captured since value was last read.
 */
typedef struct {
    volatile uint16_t value;
    volatile uint32_t capturesSinceLastRead;
} adcData_t;
#endif // ENABLE_ADC_C2000

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PROTOTYPES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

namespace Adc {

    bool Init               ();
    uint16_t ReadAdcChannel      (uint8_t adc_channel);

#if TARGET_HW_MSP432
    namespace MSP432 {
        bool Init               ();
        uint16_t ReadAdcChannel (uint8_t adc_channel);
    }
#endif // TARGET_HW_MSP432

#if ENABLE_ADC_C2000
    namespace C2000 {
        bool Init               ();
        int ReadAdcChannel      (int adc_channel);
    }
#endif // ENABLE_ADC_C2000

#if ENABLE_ADC_PYTHON
    namespace Python {
        bool Init               ();
        int ReadAdcChannel      (int adc_channel);
    }
#endif // ENABLE_ADC_PYTHON
}
#endif // _SRC_ADC
