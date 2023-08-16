#include "file_command_handler.h"
#include "command_header.h"
#include "error_checker.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

int filelist_req_send(int conn_fd, int argc, char *argv[])
{
    cmdhdr_t cmd_header = 
    {
        .header = CMD_HEADER,
        .command = CMD_REQ_FILE_LIST,
        .data_len = 0,
    };

    int ret = write(conn_fd, (uint8_t *)&cmd_header, sizeof(cmd_header));
    ERROR_CHECK(ret, "write()");

    return 0;
}

int file_req_send(int conn_fd, int argc, char *argv[])
{
    if(argc != 1)
        return -1;

    cmdhdr_t cmd_header = 
    {
        .header = CMD_HEADER,
        .command = CMD_REQ_FILE,
        .data_len = strlen(argv[0]),
    };

    int ret = write(conn_fd, (uint8_t *)&cmd_header, sizeof(cmd_header));
    ERROR_CHECK(ret, "write()");

    ret = write(conn_fd, argv[0], strlen(argv[0]));
    ERROR_CHECK(ret, "write()");

    return 0;
}

int file_rep_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    printf("file_rep_handle()\n");

    return 0;
}

int filelist_rep_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    printf(">> List of file:\n");
    for(int i = 0; i < size; i++)
    {
        printf("%c", buff[i]);
    }
    printf("\n");

    return 0;
}
