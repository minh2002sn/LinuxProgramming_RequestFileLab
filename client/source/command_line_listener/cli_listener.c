#include "cli_listener.h"
#include "cli_handler.h"
#include <stdio.h>
#include <string.h>

int clilstn_listen(int conn_fd, char* cmd)
{
    int argc = 0;
    char *argv[255];
    char *token;

    token = strtok(cmd, " ");
    while(token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    return clihdl_execute(conn_fd, argc, argv);
}
