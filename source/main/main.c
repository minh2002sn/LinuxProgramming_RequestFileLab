#include "app_main.h"

int main(int argc, char *argv[])
{
    app_main_init(argc, argv);

    while(1)
    {
        app_main_loop();
    }

    app_main_deinit();

    return 0;
}