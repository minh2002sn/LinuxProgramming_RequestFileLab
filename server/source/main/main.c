#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "app_server.h"

int main(int argc, char *argv[])
{
    // app_main_init(argc, argv);
    if (argc == 2)
    {
        app_server_init(atoi(argv[1]));
    }
    else
    {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }

    app_server_loop();

    app_server_deinit();

    return 0;
}