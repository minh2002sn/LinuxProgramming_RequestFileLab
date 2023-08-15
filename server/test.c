#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#if 1
#define ERROR_CHECK(ret, msg)                               \
    do                                                      \
    {                                                       \
        if ((ret) == -1)                                    \
        {                                                   \
            fprintf(stderr, "%s[%d] ", __FILE__, __LINE__); \
            perror(msg);                                    \
            exit(EXIT_FAILURE);                             \
        }                                                   \
    } while (0)

#define LISTEN_BACKLOG 50
#define BUFF_SIZE

typedef struct
{
    uint8_t header;
    uint8_t command; // command_type_t
    uint32_t data_len;
} __attribute__((packed)) cmdhdr_t;

int main(int argc, char *argv[])
{
    int port_no = 0;
    struct sockaddr_in server_addr = {};
    int server_fd = 0;
    int ret = 0;

    /* Read port number from command line */
    if (argc < 3)
    {
        printf("Not enough information are provided.\nCommand: ./server <server_address> <server_port_number>\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        port_no = atoi(argv[2]);
    }

    /* Init socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(server_fd, "socket()");

    /* Init server address */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    inet_pton(AF_INET, argv[1], &(server_addr.sin_addr));

    /* Connect to server */
    ret = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret, "connect()");

    /* Chat with client */
    cmdhdr_t buff = {0xAA, 0x01, 0x00};
    printf("%ld\n", sizeof(buff));
    write(server_fd, (uint8_t *)&buff, sizeof(buff));

    uint8_t rx_buff;
    int n = 0;
    while ((n = read(server_fd, &rx_buff, 1)) > 0)
    {
        printf("%c", rx_buff);
    }

    return 0;
}

#else

int main()
{

    DIR *d;
    struct dirent *dir;
    d = opendir("./input/pictures");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    return (0);
}

#endif