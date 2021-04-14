#ifndef _SRC_STATE_OF_CHARGE_API
#define _SRC_STATE_OF_CHARGE_API

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "build_switches.h"
#include "target_hw_common.h"
#include "adc_if.h"
#include <math.h>

#define BATTERY_NOMINAL_CAPACITY_MAH    110.0f
#define CURRENT_LOAD_STATIC_MA          (-58.49f)
#define MS_PER_HOUR                     (1.0f/3600000.0f) // ms * (1s / 1000 ms) * (1 min / 60 s) * (1 hr / 60 min)

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * PROTOTYPES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

namespace SocApi {

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    float getChargeFromOcv(uint16_t ocv);
    float calculateStateOfCharge(float prevSoc, float currentLoad);
}
#endif // _SRC_STATE_OF_CHARGE_API
