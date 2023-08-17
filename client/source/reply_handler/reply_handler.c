#include "reply_handler.h"
#include "message_header.h"
#include "file_command_handler.h"
#include "error_checker.h"
#include "app_config.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

static int error_req_handle(int conn_fd, uint8_t *buff, uint16_t size, ...);

typedef int (*rep_handle_func_t)(int conn_fd, uint8_t *buff, uint16_t size, ...);

typedef struct
{
    const uint8_t command; // command_type_t
    const rep_handle_func_t function;
} rephdl_table_entry_t;

static rephdl_table_entry_t rephdl_table[] =
    {
        {MSG_CMD_REP_FILE_LIST, filelist_rep_handle},
        {0, error_req_handle},
};

static rephdl_table_entry_t file_rephdl_table[] =
    {
        {MSG_CMD_REP_FILE, file_rep_handle},
        {0, error_req_handle},
};


static int error_req_handle(int conn_fd, uint8_t *buff, uint16_t size, ...)
{
    printf("Request error.\n");
    return 0;
}

void rephdl_execute(int conn_fd, uint8_t command, uint8_t *buff, uint16_t size)
{
    rephdl_table_entry_t *curr_entry = rephdl_table;
    while (curr_entry->command != 0)
    {
        if (curr_entry->command == command)
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

void rephdl_file_handle(int conn_fd, uint8_t command, uint8_t *buff, uint16_t size, uint8_t is_eof)
{
    rephdl_table_entry_t *curr_entry = file_rephdl_table;
    while (curr_entry->command != 0)
    {
        if (curr_entry->command == command)
        {
            // printf("CMD: %d\nCur: %d\n", request, curr_entry->command);
            int ret = curr_entry->function(conn_fd, buff, size, is_eof);
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
