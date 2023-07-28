#include "server.h"
#include "app_config.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#define LISTEN_BACKLOG                          50
#define BUFF_SIZE                               255
#define TEST_FILE_DIR                           "./input/picture_1.png"
#define SERVER_REQUEST                          "A"

typedef struct
{
    struct sockaddr_in server_addr;
    int server_fd;
    struct sockaddr_in client_addr;
    int client_fd;
    int client_addr_len;
} server_t;
static server_t gh_server;

void server_init(int port_no)
{
    int ret = 0;
    int opt_val = 1;

    /* Init server address */
    gh_server.server_addr.sin_family = AF_INET;
    gh_server.server_addr.sin_port = htons(port_no);
    gh_server.server_addr.sin_addr.s_addr = INADDR_ANY;
    gh_server.client_addr_len = sizeof(gh_server.client_addr);
    LOG_SOCK_INFO("Server", gh_server.server_addr);

    /* Init socket */
    gh_server.server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(gh_server.server_fd, "socket()");
    

    /* Make socket's address can be reusable */
    ret = setsockopt(gh_server.server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ERROR_CHECK(ret, "setsockopt()");

    /* Bind socket with server address */
    ret = bind(gh_server.server_fd, (struct sockaddr *)&gh_server.server_addr, sizeof(gh_server.server_addr));
    ERROR_CHECK(ret, "bind()");
}

void server_start()
{
    /* Listen to client */
    int ret = listen(gh_server.server_fd, LISTEN_BACKLOG);
    ERROR_CHECK(ret, "listen()");
}

int server_handle()
{
    int ret = 0;

    /* Accept connection with client */
    gh_server.client_fd = accept(gh_server.server_fd, (struct sockaddr *)&gh_server.client_addr,
                                (socklen_t *)&gh_server.client_addr_len);
    ERROR_CHECK(gh_server.client_fd, "accept()");
    LOG_SOCK_INFO("Client", gh_server.client_addr);

    /* Transfer test.txt file */
    int test_fd = open("./input/picture_1.png", O_RDONLY);
    ERROR_CHECK(test_fd, "open()");
    char file_buff[BUFF_SIZE];
    int n;
    while((n = read(test_fd, file_buff, BUFF_SIZE)) > 0)
    {
        ret = write(gh_server.client_fd, file_buff, n);
        ERROR_CHECK(n, "write()");
    }

    // struct stat st;
    // ret = stat("./input/test.txt", &st);
    // ERROR_CHECK(ret, "stat()");
    // sendfile(client_fd, test_fd, NULL, st.st_size);

    close(test_fd);

    close(gh_server.client_fd);
}

void server_deinit()
{
    close(gh_server.server_fd);
}

int a(int argc, char *argv[])
{
    int ret = 0;
    int port_no = 0;
    int server_fd = 0;
    struct sockaddr_in server_addr = {};
    struct sockaddr_in client_addr = {};
    unsigned long int client_addr_len = sizeof(struct sockaddr_in);
    int opt_val = 1;

    /* Read port number from command line */
    if(argc < 2)
    {
        printf("No port number is provided.\nCommand: ./server <port_number>\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        port_no = atoi(argv[1]);
    }

    /* Init socket */
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    ERROR_CHECK(server_fd, "socket()");
    
    /* Init server address */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port =  htons(port_no);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    LOG_SOCK_INFO("Server", server_addr);

    /* Prevent "Address already in use" error */
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ERROR_CHECK(ret, "setsockopt()");

    /* Bind socket with server address */
    ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret, "bind()");

    /* Receive request from client */
    printf("Waiting client.\n");
    char recv_buff[BUFF_SIZE];
    ret = recvfrom(server_fd, recv_buff, BUFF_SIZE, 0,
                (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    LOG_SOCK_INFO("Client", client_addr);
    ERROR_CHECK(ret, "recvfrom()");

    /* Transfer test.txt file */
    int test_fd = open(TEST_FILE_DIR, O_RDONLY);
    ERROR_CHECK(test_fd, "open()");
    char file_buff[BUFF_SIZE];
    int n, count = 0;
    printf("Start transfer file.\n");
    while((n = read(test_fd, file_buff, BUFF_SIZE)) > 0)
    {
        ret = sendto(server_fd, file_buff, n, 0,
                    (struct sockaddr *)&client_addr, sizeof(client_addr));
        printf("%04d. Transfer packet [%d].\n", count, n);
        ERROR_CHECK(ret, "sendto()");
        count++;
    }
    ret = sendto(server_fd, NULL, 0, 0,
                (struct sockaddr *)&client_addr, sizeof(client_addr));
    printf("%04d. Transfer packet [%d].\n", count++, n);
    ERROR_CHECK(n, "write()");
    printf("Finish transfer %d packets.\n", count);

    // struct stat st;
    // ret = stat("./input/test.txt", &st);
    // ERROR_CHECK(ret, "stat()");
    // sendfile(client_fd, test_fd, NULL, st.st_size);

    close(test_fd);

    /* Close server socket */
    close(server_fd);

    return 0;
}