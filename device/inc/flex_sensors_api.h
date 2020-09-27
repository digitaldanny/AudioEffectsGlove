#ifndef _SRC_FLEX_SENSORS_API
#define _SRC_FLEX_SENSORS_API

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* INCLUDES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

#include <stdio.h>
#include "mux.h"
#include "adc.h"

namespace FlexSensors {

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * STRUCTS
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    
    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+ 
     * Description: fingerJoints_t
     * This struct contains 10-bit ADC readings for both joints of a finger.
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    typedef struct
    {
        int mcp; // First knuckle 
        int pip; // Second knuckle
    } fingerJoints_t;

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * ENUMS
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */

    /*
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+ 
     * Description: finger_e
     * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    */
    typedef enum 
    {
        FINGER_THUMB = 0,
        FINGER_POINTER = 1,
        FINGER_MIDDLE = 2,
        FINGER_RING = 3,
        FINGER_PINKY = 4
    } finger_e;

    /*
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    * PROTOTYPES
    * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    */
    
    bool Init();
    bool GetJointsData(finger_e finger, fingerJoints_t* joints);
}

#endif // _SRC_FLEX_SENSORS_API