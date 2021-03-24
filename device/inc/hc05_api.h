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
    bool Send (char* txData);
    bool Recv (char** rxData);

#if TARGET_HW_MSP432
    namespace MSP432{
        bool SetMode (hc05Mode_e mode);
        bool Send (char* txData);
        bool Recv (char** rxData);
    }
#endif
}
#endif // _SRC_HC05_API
