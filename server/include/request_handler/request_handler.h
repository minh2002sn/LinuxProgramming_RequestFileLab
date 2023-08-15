#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <stdint.h>

void reqhdl_init();
void reqhdl_execute(int conn_fd, uint8_t request, uint8_t *buff, uint16_t size);

#endif
