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
    float pitch;
    float roll;
} dataPacket_t;

#endif // _SRC_DATA_PACKET_PROTOCOL
