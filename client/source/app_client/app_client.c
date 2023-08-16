#include "app_client.h"
#include "app_config.h"
#include "error_checker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "cli_listener.h"
#include "reply_listener.h"

#define LISTEN_BACKLOG 50

#define LOG_SOCK_INFO(name, addr)                                                                      \
    printf("\n====================\n" name ":\n    address: %s\n    port: %d\n====================\n", \
           inet_ntoa(addr.sin_addr), ntohs(addr.sin_port))

typedef struct
{
    struct sockaddr_in addr;
    int fd;
} socket_t;
static socket_t *gh_client;

void app_client_init(const char *ip_str, int port_no)
{
    if (gh_client == NULL)
    {
        gh_client = malloc(sizeof(socket_t));
    }

    /* Init socket */
    gh_client->fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(gh_client->fd, "socket()");

    /* Init server address */
    gh_client->addr.sin_family = AF_INET;
    gh_client->addr.sin_port = htons(port_no);
    inet_pton(AF_INET, ip_str, &(gh_client->addr.sin_addr));

    LOG_SOCK_INFO("Server", gh_client->addr);
}

void app_client_handle()
{
    int ret = 0;
    char cli_buff[BUFF_SIZE];
    char rx_buff[BUFF_SIZE];
    int n = 0;

    /* Connect to server */
    ret = connect(gh_client->fd, (struct sockaddr *)&gh_client->addr, sizeof(gh_client->addr));
    ERROR_CHECK(ret, "connect()");

    while(1)
    {
        /* Get command */
        memset(cli_buff, 0, BUFF_SIZE);
        printf(">> Typing command: ");
        fgets(cli_buff, BUFF_SIZE, stdin);
        /* Remove '\n' character */
        cli_buff[strlen(cli_buff) - 1] = 0;
        while(clilstn_listen(gh_client->fd, cli_buff) == -1)
        {
            memset(cli_buff, 0, BUFF_SIZE);
            printf(">> Command error.\n");
            printf(">> Typing command: ");
            fgets(cli_buff, BUFF_SIZE, stdin);
        }

        while((n = read(gh_client->fd, rx_buff, BUFF_SIZE)) > 0)
        {
            ret = replstn_handle(gh_client->fd, (uint8_t *)rx_buff, n);
            if(ret == 1)
                break;
        }

    }
}

void app_client_deinit()
{
    close(gh_client->fd);
    free(gh_client);
}
