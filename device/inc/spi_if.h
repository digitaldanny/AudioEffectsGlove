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
#include "build_switches.h"
#include "setup_target_hw.h"

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
     * NAMESPACE VARIABLES
     * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

#ifdef TARGET_HW_MSP432
    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     * DESCRIPTION: spiMasterConfig
     * Configurations for EUSCI_B0 pins (P1.5-P1.7) for 3-wire master SPI.
     *
     * DETAILS:
     * - SMCLK Clock Source
     * - SMCLK = DCO = 48MHZ
     * - SPICLK = 500khz
     * - MSB First
     * - Phase
     * - High polarity (high when inactive)
     * - 3-Wire SPI Mode
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    const eUSCI_SPI_MasterConfig spiMasterConfig =
    {
            EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
            48000000,
            12000,
            EUSCI_B_SPI_MSB_FIRST,
            EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
            EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,
            EUSCI_B_SPI_3PIN
    };

    volatile spiRxData_t spiRxData; // Contains response data from SPI slave

#endif // TARGET_HW_MSP432

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
