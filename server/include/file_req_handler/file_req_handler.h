#ifndef FILE_REQ_HANDLER_H
#define FILE_REQ_HANDLER_H

#include <stdint.h>

int filelist_req_handle(int conn_fd, uint8_t *buff, uint16_t size);
int file_req_handle(int conn_fd, uint8_t *buff, uint16_t size);

#endif
