#include "request_listener.h"
#include "app_config.h"
#include <stdint.h>

#define BUFF_SIZE                   255
#define HEADER                      0xAA

typedef enum
{
    STATE_IDLE,             // aka waiting header
    STATE_WAITING_REQUEST,
    STATE_WAITING_DATA_LEN,
    STATE_WAITING_DATA,
} request_listener_state_t;

enum
{
    REQUEST_FILES_LIST  = 0x01,
    REQUEST_FILE        = 0x02,
};

typedef struct
{
    uint8_t header;
    uint8_t request;
    uint8_t data_len;
} __attribute__((packed)) request_header_t;

typedef struct
{
    request_listener_state_t state;
    request_header_t request_header;
    uint8_t received_data_len;
    uint8_t data_buff[BUFF_SIZE];
} request_listener_t;

static request_listener_t gh_request_listener;

static void byte_handle(uint8_t recv_byte)
{
    switch(gh_request_listener.state)
    {
        case STATE_IDLE:
            if(recv_byte == HEADER)
            {
                gh_request_listener.state = STATE_WAITING_REQUEST;
            }
            break;
        case STATE_WAITING_REQUEST:

            break;
        case STATE_WAITING_DATA_LEN:

            break;
        case STATE_WAITING_DATA:

            break;
    }
}

void request_listener_listen()
{
    
}