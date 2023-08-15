#include "request_listener.h"
#include "app_config.h"
#include "request_handler.h"
#include "command_header.h"
#include <string.h>

typedef enum
{
    STATE_IDLE, // waiting header byte state
    STATE_WAITING_REQUEST,
    STATE_WAITING_DATA_LEN,
    STATE_WAITING_DATA,
} reqlstn_state_t;

typedef struct
{
    cmdhdr_t cmd_header;
    reqlstn_state_t state;
    uint32_t rx_data_len;
    uint8_t cnt_data_len_byte;
    uint8_t data_buff[BUFF_SIZE];
} reqlstn_t;

static reqlstn_t g_req_listener;

static void handle_rx_byte(int conn_fd, uint8_t recv_byte)
{
    switch (g_req_listener.state)
    {
    case STATE_IDLE:
        printf("%s: %02X\n", "STATE_IDLE", recv_byte);
        if (recv_byte == CMD_HEADER)
        {
            memset((void *)&g_req_listener, 0, sizeof(g_req_listener));
            g_req_listener.state = STATE_WAITING_REQUEST;
        }
        break;

    case STATE_WAITING_REQUEST:
        printf("%s: %02X\n", "STATE_WAITING_REQUEST", recv_byte);
        g_req_listener.cmd_header.command = recv_byte;
        g_req_listener.state = STATE_WAITING_DATA_LEN;
        break;

    case STATE_WAITING_DATA_LEN:
        printf("%s: %02X\n", "STATE_WAITING_DATA_LEN", recv_byte);
        g_req_listener.cmd_header.data_len |= (uint32_t)recv_byte
                                              << (8 * g_req_listener.cnt_data_len_byte++);
        if (g_req_listener.cnt_data_len_byte == 4)
        {
            if (g_req_listener.cmd_header.data_len != 0)
            {
                g_req_listener.state = STATE_WAITING_DATA;
            }
            else
            {
                g_req_listener.state = STATE_IDLE;
                reqhdl_execute(conn_fd, g_req_listener.cmd_header.command,
                            g_req_listener.data_buff, g_req_listener.cmd_header.data_len);
            }
        }
        break;

    case STATE_WAITING_DATA:
        g_req_listener.data_buff[g_req_listener.rx_data_len++] = recv_byte;
        if (g_req_listener.rx_data_len == g_req_listener.cmd_header.data_len)
        {
            g_req_listener.state = STATE_IDLE;
            reqhdl_execute(conn_fd, g_req_listener.cmd_header.command,
                          g_req_listener.data_buff, g_req_listener.cmd_header.data_len);
        }
        break;

    default:
        break;
    }
}

void reqlstn_handle(int conn_fd, uint8_t *buffer, uint16_t size)
{
    for (int i = 0; i < size; i++)
    {
        handle_rx_byte(conn_fd, buffer[i]);
    }
}