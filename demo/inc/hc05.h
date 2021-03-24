#ifndef _SRC_HC05_C2000
#define _SRC_HC05_C2000

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#include "driverlib.h"
#include "device.h"

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#define SCI_MAX_BUFFER_LENGTH   100

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * PROTOTYPES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// HC-05 API
void initHc05();
bool writeHc05(uint16_t* msg, uint16_t len);
bool readHc05(uint16_t** rx, uint16_t len);

// SCI drivers
__interrupt void sciCTXFIFOISR(void);
__interrupt void sciCRXFIFOISR(void);
void initSCIC(void);
void initSCICFIFO(void);

#endif // _SRC_HC05_C2000
