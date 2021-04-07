#ifndef _SRC_I2C_IF
#define _SRC_I2C_IF

#include "build_switches.h"
#include "target_hw_common.h"

typedef enum {
    eUSCI_IDLE = 0,
    eUSCI_SUCCESS = 0,
    eUSCI_BUSY = 1,
    eUSCI_NACK = 2,
    eUSCI_STOP,
    eUSCI_START
} eUSCI_status;

namespace I2c
{
    /*
     * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
     * PROTOTYPES
     * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    bool init();
    bool read(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    bool write(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

#ifdef TARGET_HW_MSP432
    namespace MSP432
    {
        bool init();
        bool read(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        bool write(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    }
#endif // TARGET_HW_MSP432
}

#endif // _SRC_I2C_IF
