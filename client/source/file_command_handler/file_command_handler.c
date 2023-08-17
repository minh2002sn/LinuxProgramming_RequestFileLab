#include "file_command_handler.h"
#include "message_header.h"
#include "error_checker.h"
#include "app_config.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

static int rx_file_fd = -1;

int filelist_req_send(int conn_fd, int argc, char *argv[])
{
    msg_header_t msg_header = 
    {
        .header = MSG_HEADER,
        .command = MSG_CMD_REQ_FILE_LIST,
        .type = MSG_TYPE_NORMAL,
        .data_len = 0,
    };

    int ret = write(conn_fd, (uint8_t *)&msg_header, sizeof(msg_header));
    ERROR_CHECK(ret, "write()");

    return 0;
}

int file_req_send(int conn_fd, int argc, char *argv[])
{
    if(argc != 1)
        return -1;

    msg_header_t msg_header = 
    {
        .header = MSG_HEADER,
        .command = MSG_CMD_REQ_FILE,
        .type = MSG_TYPE_NORMAL,
        .data_len = strlen(argv[0]),
    };

    int ret = write(conn_fd, (uint8_t *)&msg_header, sizeof(msg_header));
    ERROR_CHECK(ret, "write()");

    ret = write(conn_fd, argv[0], strlen(argv[0]));
    ERROR_CHECK(ret, "write()");

    return 0;
}

int filelist_rep_handle(int conn_fd, uint8_t *buff, uint16_t size, ...)
{
    printf(">> List of file:\n");
    for(int i = 0; i < size; i++)
    {
        printf("%c", buff[i]);
    }
    printf("\n");

    return 0;
}

int file_rep_handle(int conn_fd, uint8_t *buff, uint16_t size, ...)
{
    // printf("file_rep_handle()\n");

    int ret = 0;
    int is_eof = 0;
    va_list var_list;

    /* Get value of is_eof variable in argument list */
    va_start(var_list, size);
    is_eof = va_arg(var_list, int);
    va_end(var_list);

    /* Open file to store data */
    if(rx_file_fd == -1)
    {
        char path[255];
        sprintf(path, "%s%d_%s", OUTPUT_FOLDER, getpid(), OUTPUT_FILE_NAME);
        rx_file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
        if(rx_file_fd == -1)
        {
            ret = mkdir(OUTPUT_FOLDER, 0775);
            ERROR_CHECK(ret, "mkdir()");
            rx_file_fd = open(path, O_WRONLY | O_CREAT, 0664);
        }
        ERROR_CHECK(rx_file_fd, "open()");
    }

    /* Write data to file */
    ret = write(rx_file_fd, buff, size);
    ERROR_CHECK(ret, "write()");

    /* Close file if done receiving */
    if(is_eof)
    {
        printf("Done receiving file.\n");
        close(rx_file_fd);
        rx_file_fd = -1;
    }

    return 0;
}
