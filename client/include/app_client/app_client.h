#ifndef APP_CLIENT_H
#define APP_CLIENT_H

void app_client_init(const char *ip_str, int port_no);
void app_client_handle();
void app_client_deinit();

#endif
