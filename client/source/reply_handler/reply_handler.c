#include "reply_handler.h"
#include "command_header.h"
#include "file_command_handler.h"
#include "error_checker.h"
#include <stdio.h>
#include <unistd.h>

static int error_req_handle(int conn_fd, uint8_t *buff, uint16_t size);

typedef int (*req_handle_func_t)(int conn_fd, uint8_t *buff, uint16_t size);

typedef struct
{
    const uint8_t command; // command_type_t
    const req_handle_func_t function;
} reqhdl_table_entry_t;

static reqhdl_table_entry_t reqhdl_table[] =
    {
        {CMD_REP_FILE_LIST, filelist_rep_handle},
        // {CMD_REP_FILE, file_rep_handle},
        {0, error_req_handle},
};

// static reqhdl_table_entry_t reqhdl_table[] =
//     {
//         {CMD_REP_FILE_LIST, filelist_rep_handle},
//         {CMD_REP_FILE, file_rep_handle},
//         {0, error_req_handle},
// };

static int error_req_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    printf("Request error.\n");
    int ret = 0;
    cmdhdr_t cmd_header =
        {
            .header = CMD_HEADER,
            .command = CMD_ERROR,
            .data_len = 0,
        };

    ret = write(conn_fd, (uint8_t *)&cmd_header, sizeof(cmd_header));
    ERROR_CHECK(ret, "write()");

    return 0;
}

void rephdl_execute(int conn_fd, uint8_t request, uint8_t *buff, uint16_t size)
{
    reqhdl_table_entry_t *curr_entry = reqhdl_table;
    while (curr_entry->command != 0)
    {
        if (curr_entry->command == request)
        {
            // printf("CMD: %d\nCur: %d\n", request, curr_entry->command);
            int ret = curr_entry->function(conn_fd, buff, size);
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
