#include "reply_listener.h"
#include "app_config.h"
#include "reply_handler.h"
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
    uint8_t msg_hdr_rx_complete;
    uint8_t msg_rx_complete;
} reqlstn_t;

static reqlstn_t g_rep_listener;

static void handle_rx_byte(int conn_fd, uint8_t recv_byte)
{
    switch (g_rep_listener.state)
    {
    case STATE_IDLE:
        // printf("%s: %02X\n", "STATE_IDLE", recv_byte);
        if (recv_byte == MSG_HEADER)
        {
            g_rep_listener.state = STATE_WAITING_COMMAND;
            g_rep_listener.msg_rx_complete = 0;
        }
        break;

    case STATE_WAITING_COMMAND:
        // printf("%s: %02X\n", "STATE_WAITING_REQUEST", recv_byte);
        g_rep_listener.msg_header.command = recv_byte;
        g_rep_listener.state = STATE_WAITING_TYPE;
        break;
    
    case STATE_WAITING_TYPE:
        // printf("%s: %02X\n", "STATE_WAITING_TYPE", recv_byte);
        g_rep_listener.msg_header.type = recv_byte;
        g_rep_listener.state = STATE_WAITING_DATA_LEN;
        break;

    case STATE_WAITING_DATA_LEN:
        // printf("%s: %02X\n", "STATE_WAITING_DATA_LEN", recv_byte);
        g_rep_listener.msg_header.data_len |= (uint32_t)recv_byte
                                              << (8 * g_rep_listener.cnt_data_len_byte++);
        if (g_rep_listener.cnt_data_len_byte == 4)
        {
            g_rep_listener.msg_hdr_rx_complete = 1;
            if (g_rep_listener.msg_header.data_len != 0)
            {
                g_rep_listener.state = STATE_WAITING_DATA;
            }
            else
            {
                rephdl_execute(conn_fd, g_rep_listener.msg_header.command,
                            g_rep_listener.data_buff, g_rep_listener.msg_header.data_len);
                memset((void *)&g_rep_listener, 0, sizeof(g_rep_listener));
                g_rep_listener.state = STATE_IDLE;
                g_rep_listener.msg_rx_complete = 1;
            }
        }
        break;

    case STATE_WAITING_DATA:
        // printf("%s: %c\n", "STATE_WAITING_DATA", recv_byte);
        g_rep_listener.data_buff[g_rep_listener.rx_data_len++] = recv_byte;
        if (g_rep_listener.rx_data_len == g_rep_listener.msg_header.data_len)
        {
            rephdl_execute(conn_fd, g_rep_listener.msg_header.command,
                          g_rep_listener.data_buff, g_rep_listener.msg_header.data_len);
            memset((void *)&g_rep_listener, 0, sizeof(g_rep_listener));
            g_rep_listener.state = STATE_IDLE;
            g_rep_listener.msg_rx_complete = 1;
        }
        break;

    default:
        break;
    }
}

int replstn_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    // printf("size arg: %d, state: %d, data len: %d\n", size, g_rep_listener.state, g_rep_listener.rx_data_len);
    if (g_rep_listener.msg_hdr_rx_complete &&
            g_rep_listener.msg_header.type == MSG_TYPE_FILE)
    {
        int is_eof = 0;

        g_rep_listener.rx_data_len += size;
        is_eof = (g_rep_listener.rx_data_len >= g_rep_listener.msg_header.data_len) ? 1 : 0;
        rephdl_file_handle(conn_fd, g_rep_listener.msg_header.command, buff, size, is_eof);
        
        if(is_eof)
        {
            memset((void *)&g_rep_listener, 0, sizeof(g_rep_listener));
            g_rep_listener.state = STATE_IDLE;
            g_rep_listener.msg_rx_complete = 1;
        }
    }
    else
    {
        for (int i = 0; i < size; i++)
        {
            handle_rx_byte(conn_fd, buff[i]);
            if (g_rep_listener.msg_hdr_rx_complete &&
                g_rep_listener.msg_header.type == MSG_TYPE_FILE)
            {
                buff += i + 1;
                size -= i + 1;
                g_rep_listener.rx_data_len += size;
                int is_eof = (g_rep_listener.rx_data_len >= g_rep_listener.msg_header.data_len) ? 1 : 0;
                rephdl_file_handle(conn_fd, g_rep_listener.msg_header.command, buff, size, is_eof);
                if(is_eof)
                {
                    memset((void *)&g_rep_listener, 0, sizeof(g_rep_listener));
                    g_rep_listener.state = STATE_IDLE;
                    g_rep_listener.msg_rx_complete = 1;
                }
                break;
            }
        }
    }
    return g_rep_listener.msg_rx_complete;
}