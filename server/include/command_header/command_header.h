#ifndef COMMAND_HEADER_H
#define COMMAND_HEADER_H

#include <stdint.h>

#define CMD_HEADER 0xAA

typedef enum
{
    CMD_REQ_FILE_LIST = 0x01,
    CMD_REQ_FILE,
    CMD_REP_FILE_LIST,
    CMD_REP_FILE,
} command_type_t;

typedef struct
{
    uint8_t header;
    uint8_t command; // command_type_t
    uint32_t data_len;
} __attribute__((packed)) cmdhdr_t;

#endif
