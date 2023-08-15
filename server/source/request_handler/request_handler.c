#include "request_handler.h"
#include "command_header.h"
#include "file_req_handler.h"
#include <stdio.h>

typedef void (*req_handle_func_t)(int conn_fd, uint8_t *buff, uint16_t size);

typedef struct
{
    const uint8_t command; // command_type_t
    const req_handle_func_t function;
} req_handler_entry_t;

static req_handler_entry_t req_handler_table[] =
    {
        {CMD_REQ_FILE_LIST, filelist_req_handle},
        {CMD_REQ_FILE, file_req_handle},
        {0, NULL}};

void reqhdl_execute(int conn_fd, uint8_t request, uint8_t *buff, uint16_t size)
{
    req_handler_entry_t *curr_entry = req_handler_table;
    while (curr_entry->command != 0)
    {
        if (curr_entry->command == request)
        {
            curr_entry->function(conn_fd, buff, size);
            break;
        }
        else
        {
            curr_entry++;
        }
    }
}
