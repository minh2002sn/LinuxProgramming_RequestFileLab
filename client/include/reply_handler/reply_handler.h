#ifndef REPLY_HANDLER_H
#define REPLY_HANDLER_H

#include <stdint.h>

void rephdl_init();
void rephdl_execute(int conn_fd, uint8_t command, uint8_t *buff, uint16_t size);
void rephdl_file_handle(int conn_fd, uint8_t command, uint8_t *buff, uint16_t size, uint8_t is_eof);

#endif
