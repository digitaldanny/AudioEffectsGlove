#ifndef _SRC_DATA_PACKET_PROTOCOL
#define _SRC_DATA_PACKET_PROTOCOL

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* DEFINES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// Op codes
#define DPP_OPCODE_CONNECT      0   // Master to slave - Request to connect
#define DPP_OPCODE_DISCONNECT   1   // Master to slave - Request to disconnect
#define DPP_OPCODE_ACK          2   // Slave to master - Message decoded and valid
#define DPP_OPCODE_NACK         3   // Slave to master - Error decoding message
#define DPP_OPCODE_PACKET       4   // Master to slave - New data packet containing sensor readings

// Misc
#define DPP_PACKET_NUM_BYTES    1   // Number of bytes that should be expected for a data packet update
                                    // Opcode (1) + joints 0-2 (3) + ? + suffix (1)

#define DPP_FLOAT_LENGTH_TOTAL      6   // Total number of bytes for each string representation of a float
                                        // 1=negative sign, 3=0-180 degrees, 1=dot, 1=decimal place
#define DPP_FLOAT_LENGTH_DECIMAL    1   // Out of the total length of the float, how many hold the decimal?

/*
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
* STRUCT TYPES
* +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
*/

// NOTE - unsigned char = 8 bits on MSP432, 16 bits on TMS320
typedef struct
{
    unsigned char opCode;
    unsigned char flexSensors[3];
    signed short pitch;
    signed short roll;
} dataPacket_t;

#endif // _SRC_DATA_PACKET_PROTOCOL
