#ifndef REQUEST_LISTENER_H
#define REQUEST_LISTENER_H
#include <stdint.h>

void reqlstn_handle(int conn_fd, uint8_t *buffer, uint16_t size);

#endif
