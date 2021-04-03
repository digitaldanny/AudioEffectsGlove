#ifndef _SRC_HC05_API
#define _SRC_HC05_API

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include "build_switches.h"
#include "target_hw_common.h"
#include "uart_if.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* ENUMS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

typedef enum
{
    HC05MODE_DATA,
    HC05MODE_CMD
} hc05Mode_e;

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * PROTOTYPES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

namespace Hc05Api {

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    bool SetMode (hc05Mode_e mode);
    bool IsSlaveConnected();
    bool Send (char* txData, uint8_t numBytes);
    bool Recv (char** rxData, uint8_t numBytes);

#if TARGET_HW_MSP432
    namespace MSP432{
        bool SetMode (hc05Mode_e mode);
        bool IsSlaveConnected();
        bool Send (char* txData, uint8_t numBytes);
        bool Recv (char** rxData, uint8_t numBytes);
    }
#endif
}
#endif // _SRC_HC05_API
