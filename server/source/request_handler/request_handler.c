#include "request_handler.h"
#include "message_header.h"
#include "file_req_handler.h"
#include "error_checker.h"
#include <stdio.h>
#include <unistd.h>

static int error_req_handle(int conn_fd, uint8_t *buff, uint16_t size, ...);

typedef int (*req_handle_func_t)(int conn_fd, uint8_t *buff, uint16_t size, ...);

typedef struct
{
    const uint8_t command; // command_type_t
    const req_handle_func_t function;
} reqhdl_table_entry_t;

static reqhdl_table_entry_t reqhdl_table[] =
    {
        {MSG_CMD_REQ_FILE_LIST, filelist_req_handle},
        {MSG_CMD_REQ_FILE, file_req_handle},
        {0, error_req_handle}};

static int error_req_handle(int conn_fd, uint8_t *buff, uint16_t size, ...)
{
    printf("Request error.\n");
    int ret = 0;
    msg_header_t msg_header = 
    {
        .header = MSG_HEADER,
        .command = MSG_CMD_ERROR,
        .type = MSG_TYPE_NORMAL,
        .data_len = 0,
    };

    ret = write(conn_fd, (uint8_t *)&msg_header, sizeof(msg_header));
    ERROR_CHECK(ret, "write()");

    return 0;
}

void reqhdl_execute(int conn_fd, uint8_t request, uint8_t *buff, uint16_t size)
{
    reqhdl_table_entry_t *curr_entry = reqhdl_table;
    while (curr_entry->command != 0)
    {
        if (curr_entry->command == request)
        {
            // printf("CMD: %d\nCur: %d\n", request, curr_entry->command);
            int ret = curr_entry->function(conn_fd, buff, size);
            printf("Done handling request.\n");
            if (ret == -1)
            {
                error_req_handle(conn_fd, NULL, 0);
            }
            return;
        }
        else
        {
            curr_entry++;
        }
    }
    error_req_handle(conn_fd, NULL, 0);
}
