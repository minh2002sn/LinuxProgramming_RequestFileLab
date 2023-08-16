#ifndef REPLY_HANDLER_H
#define REPLY_HANDLER_H

#include <stdint.h>

void rephdl_init();
void rephdl_execute(int conn_fd, uint8_t request, uint8_t *buff, uint16_t size);

#endif
