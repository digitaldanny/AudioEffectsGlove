/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DESCRIPTION: spi_if.h
 * This file contains prototypes and data structures for configuring/utilizing
 * 3-pin master SPI.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#ifndef INC_SPI_IF_H_
#define INC_SPI_IF_H_

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#include <target_hw_common.h>
#include "build_switches.h"

namespace Spi
{
    /*
     * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
     * STRUCT DEFINITIONS
     * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: spiRxData_t
     * Struct for storing data received from SPI slave.
     *
     * Field Description:
     * @flagSpiReady - True if data has been received and stored into 'data' field.
     * @data - Data received from the SPI slave.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    typedef struct  {
        bool flagSpiReady;
        uint8_t data;
    } spiRxData_t;

    /*
     * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
     * PROTOTYPES
     * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    void init();
    uint8_t transferByte(uint8_t data);

#ifdef TARGET_HW_MSP432
    namespace MSP432
    {
        void init();
        uint8_t transferByte(uint8_t data);
    }
#endif // TARGET_HW_MSP432
}

#endif /* INC_SPI_IF_H_ */
