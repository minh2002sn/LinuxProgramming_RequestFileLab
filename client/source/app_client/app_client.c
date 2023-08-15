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
    int recv_file_fd = 0;
    char rx_buff[BUFF_SIZE];
    int n = 0;
    long unsigned int num_rx_byte = 0;

    /* Connect to server */
    ret = connect(gh_client->fd, (struct sockaddr *)&gh_client->addr, sizeof(gh_client->addr));
    ERROR_CHECK(ret, "connect()");

    /* Open storing rx data file */
    recv_file_fd = open(OUTPUT_FOLDER OUTPUT_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(recv_file_fd == -1)
    {
        ret = mkdir(OUTPUT_FOLDER, 0775);
        ERROR_CHECK(ret, "mkdir()");
        recv_file_fd = open(OUTPUT_FOLDER OUTPUT_FILE_NAME, O_WRONLY | O_CREAT, 0666);
    }
    ERROR_CHECK(recv_file_fd, "open()");

    /* Receiving data */
    while((n = read(gh_client->fd, rx_buff, BUFF_SIZE)) > 0)
    {
        ret = write(recv_file_fd, rx_buff, BUFF_SIZE);
        ERROR_CHECK(ret, "write()");
        num_rx_byte += n;
    }
    ERROR_CHECK(n, "read()");
    printf("Done receive %ld bytes.\n", num_rx_byte);

    close(recv_file_fd);
}

void app_client_deinit()
{
    close(gh_client->fd);
    free(gh_client);
}
