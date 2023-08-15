#ifndef FILE_REQ_HANDLER_H
#define FILE_REQ_HANDLER_H

#include <stdint.h>

void filelist_req_handle(int conn_fd, uint8_t *buff, uint16_t size);
void file_req_handle(int conn_fd, uint8_t *buff, uint16_t size);

#endif
