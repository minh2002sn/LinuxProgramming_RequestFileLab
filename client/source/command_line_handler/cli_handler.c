#include "cli_handler.h"
#include "file_command_handler.h"
#include <stdio.h>
#include <string.h>

typedef int (*req_handle_func_t)(int conn_fd, int argc, char *argv[]);

typedef struct
{
    const char *command;
    const req_handle_func_t function;
} reqhdl_table_entry_t;

static reqhdl_table_entry_t clihdl_table[] =
    {
        {"filelist", filelist_req_send},
        {"file", file_req_send},
        {NULL, NULL},
};

int clihdl_execute(int conn_fd, int argc, char *argv[])
{
    reqhdl_table_entry_t *curr_entry = clihdl_table;
    while(curr_entry->command != NULL)
    {
        // printf("CMD: %s\nCur: %s\n", argv[0], curr_entry->command);
        if(strstr(argv[0], curr_entry->command) != NULL)
        {
            int ret = curr_entry->function(conn_fd, argc - 1, argv + 1);
            if(ret == -1)
                return -1;
            return 0;
        }
        else
        {
            curr_entry++;
        }
    }
    return -1;
}
