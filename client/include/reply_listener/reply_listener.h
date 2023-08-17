#ifndef REPLY_LISTENER_H
#define REPLY_LISTENER_H
#include <stdint.h>

int replstn_handle(int conn_fd, uint8_t *buff, uint16_t size);

#endif
