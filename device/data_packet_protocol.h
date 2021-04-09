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

// TODO: Pad or pack the dataPacket_t struct so this value does not need to be hardcoded
// while running on C2000. It would be better to use 'sizeof(dataPacket_t)' instead of
// calling this.
#define DPP_PACKET_SIZE_IN_BYTES_C2000    (8) // 1=opCode, 3=flexSensors, 2=pitch, 2=roll

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
    short pitch;
    short roll;
} dataPacket_t;

#endif // _SRC_DATA_PACKET_PROTOCOL
