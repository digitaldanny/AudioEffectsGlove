#ifndef _SRC_HC05_C2000
#define _SRC_HC05_C2000

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * INCLUDES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/
#include "driverlib.h"
#include "device.h"
#include <string.h>

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * DEFINES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#define ENABLE_SCIC_LOOPBACK_TEST   0
#define ENABLE_HC05_NAME_TEST       1

#define BAUDRATE_38400      (38400<<1)
#define BAUDRATE_9600       (9600<<1)
#define BAUDRATE_DEFAULT    BAUDRATE_38400

#define SCI_MAX_BUFFER_LENGTH       100

/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * PROTOTYPES
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Unit tests
#if ENABLE_SCIC_LOOPBACK_TEST
bool loopbackTest();
#endif // ENABLE_SCIC_LOOPBACK_TEST
#if ENABLE_HC05_NAME_TEST
bool hc05NameTest();
#endif // ENABLE_HC05_NAME_TEST

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
