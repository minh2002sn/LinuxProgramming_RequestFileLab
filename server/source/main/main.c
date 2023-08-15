#include "main.h"
#include "app_main.h"

int main(int argc, char *argv[])
{
    app_main_init(argc, argv);

    app_main_handle();

    app_main_deinit();

    return 0;
}