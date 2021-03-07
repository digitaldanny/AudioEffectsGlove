#ifndef _SRC_MUX
#define _SRC_MUX

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* INCLUDES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Common target includes
#include "build_switches.h"
#include <stdio.h>

#if TARGET_HW_PYTHON_CAPABLE
#include "python_common.h"

#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif // TARGET_HW_RASPBERRYPI

#if TARGET_HW_DESKTOP
#include "python3.6/Python.h"
#endif // TARGET_HW_DESKTOP
#endif // TARGET_HW_PYTHON_CAPABLE

#if TARGET_HW_C2000
extern "C" {
 #include <device_headers/device.h>
}
#endif

#if TARGET_HW_MSP432
 #include "setup_target_hw.h"
#endif // TARGET_HW_MSP432

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* DEFINES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#define MUX_CHAN_MIN        (0)   // Minimum mux channel
#define MUX_CHAN_MAX        (7)   // Maximum mux channel
#define MUX_NUM_SEL_LINES   (3)   // Number of select lines (sel[2:0] = 3)

#define MUX_MODULE          PROTO_PATH "cd4051be_mux"
#define MUX_CLASS           "Cd4051be"
#define MUX_INIT_FUNC        "__enter__"
#define MUX_SET_CH_FUNC     "setMuxCh"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PROTOTYPES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

namespace Mux {

    bool Init               ();
    bool SelectMuxChannel   (int mux_channel);

#if TARGET_HW_MSP432
    namespace MSP432 {
        bool Init               ();
        bool SelectMuxChannel   (int mux_channel);
    }
#endif // ENABLE_TARGET_MSP432

#if TARGET_HW_C2000
    namespace C2000 {
        bool Init               ();
        bool SelectMuxChannel   (int mux_channel);
    }
#endif // TARGET_HW_C2000

#if TARGET_HW_PYTHON_CAPABLE
    namespace Python {
        bool Init               ();
        bool SelectMuxChannel   (int mux_channel);
    }
#endif // ENABLE_MUX_PYTHON
}

#endif
