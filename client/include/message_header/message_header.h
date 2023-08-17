#ifndef MESSAGE_HEADER_H
#define MESSAGE_HEADER_H

#include <stdint.h>

#define MSG_HEADER 0xAA // message header

typedef enum
{
    MSG_CMD_REQ_FILE_LIST = 0x01, // request list of files
    MSG_CMD_REQ_FILE,             // request file
    MSG_CMD_REP_FILE_LIST,        // reply list of files
    MSG_CMD_REP_FILE,             // reply file

    MSG_CMD_ERROR = 0xFF,
} cmd_type_t;

typedef enum
{
    MSG_TYPE_NORMAL = 0x01, // normal message type
    MSG_TYPE_FILE,          // message include a file
} msg_type_t;

typedef struct
{
    uint8_t header;         // header
    cmd_type_t command : 8; // command_type_t
    msg_type_t type : 8;    // type of message
    uint32_t data_len;      // data length
} __attribute__((packed)) msg_header_t;

#endif
