#include "request_listener.h"
#include "app_config.h"
#include "request_handler.h"
#include "message_header.h"
#include <stdio.h>
#include <string.h>

typedef enum
{
    STATE_IDLE, // waiting header byte state
    STATE_WAITING_COMMAND,
    STATE_WAITING_TYPE,
    STATE_WAITING_DATA_LEN,
    STATE_WAITING_DATA,
} reqlstn_state_t;

typedef struct
{
    msg_header_t msg_header;
    reqlstn_state_t state;
    uint32_t rx_data_len;
    uint8_t cnt_data_len_byte;
    uint8_t data_buff[BUFF_SIZE];
    uint8_t msg_hdr_tx_complete;
    uint8_t msg_tx_complete;
} reqlstn_t;

static reqlstn_t g_req_listener;

static void handle_rx_byte(int conn_fd, uint8_t recv_byte)
{
    switch (g_req_listener.state)
    {
    case STATE_IDLE:
        printf("%s: %02X\n", "STATE_IDLE", recv_byte);
        if (recv_byte == MSG_HEADER)
        {
            memset((void *)&g_req_listener, 0, sizeof(g_req_listener));
            g_req_listener.state = STATE_WAITING_COMMAND;
        }
        break;

    case STATE_WAITING_COMMAND:
        printf("%s: %02X\n", "STATE_WAITING_COMMAND", recv_byte);
        g_req_listener.msg_header.command = recv_byte;
        g_req_listener.state = STATE_WAITING_TYPE;
        break;

    case STATE_WAITING_TYPE:
        printf("%s: %02X\n", "STATE_WAITING_TYPE", recv_byte);
        g_req_listener.msg_header.type = recv_byte;
        g_req_listener.msg_hdr_tx_complete = 1;
        g_req_listener.state = STATE_WAITING_DATA_LEN;
        break;

    case STATE_WAITING_DATA_LEN:
        printf("%s: %02X\n", "STATE_WAITING_DATA_LEN", recv_byte);
        g_req_listener.msg_header.data_len |= (uint32_t)recv_byte
                                              << (8 * g_req_listener.cnt_data_len_byte++);
        if (g_req_listener.cnt_data_len_byte == 4)
        {
            if (g_req_listener.msg_header.data_len != 0)
            {
                g_req_listener.state = STATE_WAITING_DATA;
            }
            else
            {
                g_req_listener.state = STATE_IDLE;
                g_req_listener.msg_tx_complete = 1;
                reqhdl_execute(conn_fd, g_req_listener.msg_header.command,
                               g_req_listener.data_buff, g_req_listener.msg_header.data_len);
            }
        }
        break;

    case STATE_WAITING_DATA:
        g_req_listener.data_buff[g_req_listener.rx_data_len++] = recv_byte;
        printf("%s: %c\n", "STATE_WAITING_DATA", recv_byte);
        if (g_req_listener.rx_data_len == g_req_listener.msg_header.data_len)
        {
            g_req_listener.state = STATE_IDLE;
            g_req_listener.msg_tx_complete = 1;
            reqhdl_execute(conn_fd, g_req_listener.msg_header.command,
                           g_req_listener.data_buff, g_req_listener.msg_header.data_len);
        }
        break;

    default:
        break;
    }
}

int reqlstn_handle(int conn_fd, uint8_t *buffer, uint16_t size)
{
    for (int i = 0; i < size; i++)
    {
        handle_rx_byte(conn_fd, buffer[i]);
    }

    return g_req_listener.msg_tx_complete;
}