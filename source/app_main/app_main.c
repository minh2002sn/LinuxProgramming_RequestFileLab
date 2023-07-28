#include "app_main.h"
#include "app_config.h"
#include "main.h"
#include <stdlib.h>

#if SERVER_OR_CLIENT
#include "server.h"
#else
#include "client.h"
#endif

void app_main_init(int argc, char *argv[])
{
#if SERVER_OR_CLIENT
    if(argc == 2)
    {
        server_init(atoi(argv[1]));
    }
    else
    {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }
#else
    if(argc == 3)
    {
        client_init(argv[1], atoi(argv[2]));
    }
    else
    {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }
#endif
}

void app_main_loop()
{
#if SERVER_OR_CLIENT
    server_handle();
#else
    client_handle();
#endif
}

void app_main_deinit()
{
#if SERVER_OR_CLIENT
    server_deinit();
#else
    client_handle();
#endif
}
