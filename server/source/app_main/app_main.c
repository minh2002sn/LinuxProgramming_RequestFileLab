#include "app_main.h"
#include "app_config.h"
#include "app_server.h"
#include <stdio.h>
#include <stdlib.h>

void app_main_init(int argc, char *argv[])
{
    if(argc == 2)
    {
        app_server_init(atoi(argv[1]));
    }
    else
    {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }
}

void app_main_handle()
{
    app_server_handle();
}

void app_main_deinit()
{ 
    app_server_deinit();
}
