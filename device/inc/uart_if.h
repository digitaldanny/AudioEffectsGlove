#ifndef _SRC_UART
#define _SRC_UART

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* INCLUDES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Common target includes
#include "build_switches.h"
#include "target_hw_common.h"

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* DEFINES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// MSP432 SPECIFIC
#define UART_BRDIV_CLK_12M_BAUDRATE_38400           19
#define UART_UCXBRF_CLK_12M_BAUDRATE_38400          8
#define UART_UCXBRS_CLK_12M_BAUDRATE_38400          85
#define UART_OVERSAMPLING_CLK_12M_BAUDRATE_38400    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION

#define UART_BRDIV_CLK_12M_BAUDRATE_115200           6
#define UART_UCXBRF_CLK_12M_BAUDRATE_115200          8
#define UART_UCXBRS_CLK_12M_BAUDRATE_115200          32
#define UART_OVERSAMPLING_CLK_12M_BAUDRATE_115200    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION

#define UART_BRDIV_CLK_12M_BAUDRATE_9600           78
#define UART_UCXBRF_CLK_12M_BAUDRATE_9600          2
#define UART_UCXBRS_CLK_12M_BAUDRATE_9600          0
#define UART_OVERSAMPLING_CLK_12M_BAUDRATE_9600    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* ENUMS
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
typedef enum
{
    BAUDRATE_38400,
    BAUDRATE_9600
} baudRate_e;

namespace Uart {

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    bool init (baudRate_e baudRate);
    bool send (char* txData);
    bool recv (char* rxData);

#if TARGET_HW_MSP432
    namespace MSP432 {
        bool init (const eUSCI_UART_Config* config);
        bool send (char* txData);
        bool recv (char* rxData);
    }
#endif // ENABLE_UART_C2000
}

#endif // _SRC_UART
