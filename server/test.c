#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>


#define CODE    1

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

#if CODE == 0

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
    cmdhdr_t tx_hdr_buff = {0xAA, 0x01, 0x00};
    write(server_fd, (uint8_t *)&tx_hdr_buff, sizeof(tx_hdr_buff));

    cmdhdr_t rx_hdr_buff;
    uint8_t rx_data_buff;
    int n = 0;
    int count = 0;
    while ((n = read(server_fd, &rx_data_buff, 1)) > 0)
    {
        count++;
        if(count <= sizeof(cmdhdr_t))
        {
            *((uint8_t *)&rx_hdr_buff + count - 1) = rx_data_buff;
            printf("%02X\n", rx_data_buff);
            if(count == sizeof(cmdhdr_t))
            {
                printf("len: %d\n", rx_hdr_buff.data_len);
            }
        }
        else if (count < rx_hdr_buff.data_len + sizeof(cmdhdr_t))
        {
            printf("%c", rx_data_buff);
            // printf(" count = %d\n", count);
        }
        else if (count == rx_hdr_buff.data_len + sizeof(cmdhdr_t))
        {
            printf("%c", rx_data_buff);
            break;
        }
    }
    // printf("n=%d\trx_data_buff=%d\n", n, rx_data_buff);
    close(server_fd);

    return 0;
}

#elif CODE == 1

int main(int argc, char *argv[])
{
    int port_no = 0;
    struct sockaddr_in server_addr = {};
    int server_fd = 0;
    int ret = 0;

    /* Read port number from command line */
    if (argc < 4)
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
    cmdhdr_t tx_hdr_buff = {0xAA, 0x02, strlen(argv[3])};
    write(server_fd, (uint8_t *)&tx_hdr_buff, sizeof(tx_hdr_buff));
    write(server_fd, (uint8_t *)argv[3], strlen(argv[3]));

    cmdhdr_t rx_hdr_buff;
    uint8_t rx_data_buff[1024];
    int n = 0;
    int count = 0;
    while ((n = read(server_fd, rx_data_buff, 1)) > 0)
    {
        count++;
        if(count <= sizeof(cmdhdr_t))
        {
            *((uint8_t *)&rx_hdr_buff + count - 1) = *rx_data_buff;
            printf("%02X\n", *rx_data_buff);
            if(count == sizeof(cmdhdr_t))
            {
                printf("len: %d\n", rx_hdr_buff.data_len);
                if(rx_hdr_buff.command == 0xFF)
                {
                    printf("Error.\n");
                    return 1;
                }
                break;
            }
        }
    }

    /* Open storing rx data file */
    printf("Waiting for file.\n");
    int recv_file_fd = open("./rx_file.png", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    unsigned long int num_rx_byte = 0;
    // if(recv_file_fd == -1)
    // {
    //     ret = mkdir(OUTPUT_FOLDER, 0775);
    //     ERROR_CHECK(ret, "mkdir()");
    //     recv_file_fd = open(OUTPUT_FOLDER OUTPUT_FILE_NAME, O_WRONLY | O_CREAT, 0666);
    // }
    ERROR_CHECK(recv_file_fd, "open()");

    /* Receiving data */
    while((n = read(server_fd, rx_data_buff, 1024)) > 0)
    {
        ret = write(recv_file_fd, rx_data_buff, 1024);
        ERROR_CHECK(ret, "write()");
        num_rx_byte += n;
        if(num_rx_byte >= rx_hdr_buff.data_len)
            break;
    }
    ERROR_CHECK(n, "read()");
    printf("Done receive %ld bytes.\n", num_rx_byte);

    close(recv_file_fd);

    close(server_fd);

    return 0;
}

#elif CODE == 2

#include <sys/stat.h>

int main()
{

    DIR *dir;
    struct dirent *entry;
    struct stat st;
    dir = opendir("./input/pictures");
    if (dir)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
            {
                char abs_path[1024];
                sprintf(abs_path, "./input/pictures/%s", entry->d_name);
                int ret = stat(abs_path, &st);
                ret = stat(abs_path, &st);
                if (ret == -1 || !(S_IRUSR & st.st_mode) ||
                    !(S_IRGRP & st.st_mode) || !(S_IROTH & st.st_mode))
                    continue;
                printf("Can read ");
                printf("%s\n", entry->d_name);
            }
        }
        closedir(dir);
    }
    return (0);
}

#elif CODE == 3

void func(char *argv[])
{
    printf("arg[2]: %s\n", argv[1]);
}

int main(int argc, char *argv[])
{
    func(argv + 1);
}

#endif