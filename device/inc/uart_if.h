#ifndef _SRC_UART
#define _SRC_UART

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* INCLUDES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Common target includes
#include "build_switches.h"
#include <stdio.h>

#if ENABLE_UART_C2000
extern "C" {
 #include <device_headers/device.h>
}
#endif

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* PROTOTYPES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

namespace Uart {

    bool Init               ();

#if ENABLE_UART_C2000
    namespace C2000 {
        bool Init               ();
        int Send                (uint16_t data);
        int Recv                (uint16_t* data);
    }
#endif // ENABLE_UART_C2000
}

#endif // _SRC_UART
