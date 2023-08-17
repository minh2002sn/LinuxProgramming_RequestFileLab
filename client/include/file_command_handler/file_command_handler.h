#ifndef FILE_COMMAND_HANDLER_H
#define FILE_COMMAND_HANDLER_H

#include <stdint.h>

int filelist_req_send(int conn_fd, int argc, char *argv[]);
int file_req_send(int conn_fd, int argc, char *argv[]);
int filelist_rep_handle(int conn_fd, uint8_t *buff, uint16_t size, ...);
int file_rep_handle(int conn_fd, uint8_t *buff, uint16_t size, ...);

#endif
