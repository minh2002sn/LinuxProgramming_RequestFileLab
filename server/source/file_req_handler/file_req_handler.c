#include "file_req_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "command_header.h"
#include "linked_list.h"
#include "app_config.h"
#include "error_checker.h"

/**
 * Entry structure of file lists (g_file_list)
 *
 * ---------------------------------
 * |    n bytes    |       |       |
 * ---------------------------------
 * |   File name   | '\n'  |  '\0' |
 * ---------------------------------
 */
#define MAX_NUM_FILE 2147483647 // (2**31 - 1)

static llist_t *g_file_list = NULL;
static uint16_t total_entry_size = 0;

static void file_list_update()
{
    DIR *dir;
    struct dirent *entry;

    /* Open directory */
    dir = opendir(DATA_FOLDER);
    if (dir)
    {
        if (g_file_list == NULL)
        {
            g_file_list = llist_init();
        }
        else
        {
            /* Clean up old data. */
            while (!llist_is_empty(g_file_list))
            {
                free(llist_pop(g_file_list));
            }
        }

        total_entry_size = 0;
        while ((entry = readdir(dir)) != NULL &&
               llist_get_num_node(g_file_list) < MAX_NUM_FILE)
        {
            /* Remove ". and ".." directory */
            if (entry->d_name[0] != '.')
            {
                char file_path[1024];
                int ret = 0;
                struct stat st;
                char *file_name;

                sprintf(file_path, "%s%s", DATA_FOLDER, entry->d_name);
                // printf("%s\n", file_path);
                ret = stat(file_path, &st);
                ERROR_CHECK(ret, "stat()");
                if (ret == -1 || !(S_IRUSR & st.st_mode) ||
                    !(S_IRGRP & st.st_mode) || !(S_IROTH & st.st_mode))
                    continue;

                file_name = (char *)malloc(strlen(entry->d_name) + 2);
                sprintf(file_name, "%s\n", entry->d_name);
                llist_add(g_file_list, (void *)file_name);
                total_entry_size += strlen(file_name);
                // printf("data len: %d\n", total_entry_size);
            }
        }

        /* Close directory */
        closedir(dir);
    }
    else
    {
        exit(EXIT_FAILURE);
    }
}

int filelist_req_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    int ret = 0;
    cmdhdr_t cmd_header;
    int opt;

    /* Update list of files */
    file_list_update();

    /* Create header */
    cmd_header.header = CMD_HEADER;
    cmd_header.command = CMD_REP_FILE_LIST;
    cmd_header.data_len = total_entry_size;
    // printf("data len: %d\n", total_entry_size);

    /* Enable TCP_CORK option, subsequent TCP output is corked until this option is disabled. */
    opt = 1;
    ret = setsockopt(conn_fd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));
    ERROR_CHECK(ret, "setsockopt()");

    /* Send header */
    ret = write(conn_fd, (uint8_t *)&cmd_header, sizeof(cmd_header));
    ERROR_CHECK(ret, "write()");

    /* Send list of files */
    for (int i = 0; i < llist_get_num_node(g_file_list); i++)
    {
        char *curr_file_name = (char *)llist_get_data(g_file_list, i);
        write(conn_fd, curr_file_name, strlen(curr_file_name));
    }

    /* Disable TCP_CORK option */
    opt = 0;
    ret = setsockopt(conn_fd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));
    ERROR_CHECK(ret, "setsockopt()");

    return 0;
}

int file_req_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    int file_seq_no = 0;
    int test_fd = 0;
    int ret = 0;
    struct stat st;
    cmdhdr_t cmd_header;
    char file_path[255];
    int opt = 1;

    if(size == 0)
    {
        return -1;
    }

    if(g_file_list == NULL)
    {
        file_list_update();
    }

    for(file_seq_no = 0; file_seq_no < llist_get_num_node(g_file_list); file_seq_no++)
    {
        // printf("REQ: %s\n", buff);
        // printf("%d. %s\n", file_seq_no, llist_get_data(g_file_list, file_seq_no));
        if(strstr(llist_get_data(g_file_list, file_seq_no), (char *)buff) != NULL)
        {
            break;
        }
        else if(file_seq_no == llist_get_num_node(g_file_list) - 1)
        {
            return -1;
        }
    }

    /* Get size of file */
    sprintf(file_path, "%s%s", DATA_FOLDER, (char *)buff);
    // printf("%s\n", file_path);
    ret = stat(file_path, &st);
    ERROR_CHECK(ret, "stat()");

    /* Create header */
    cmd_header.header = CMD_HEADER;
    cmd_header.command = CMD_REP_FILE;
    cmd_header.data_len = st.st_size;

    /* Enable TCP_CORK option, subsequent TCP output is corked until this option is disabled. */
    opt = 1;
    ret = setsockopt(conn_fd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));
    ERROR_CHECK(ret, "setsockopt()");

    /* Send header */
    ret = write(conn_fd, (uint8_t *)&cmd_header, sizeof(cmd_header));
    ERROR_CHECK(ret, "write()");

    /* Open file */
    test_fd = open(file_path, O_RDONLY);
    ERROR_CHECK(test_fd, "open()");

    /* Send fie */
    ret = sendfile(conn_fd, test_fd, NULL, st.st_size);
    ERROR_CHECK(ret, "sendfile()");

    /* Disable TCP_CORK option */
    opt = 0;
    ret = setsockopt(conn_fd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));
    ERROR_CHECK(ret, "setsockopt()");

    /* Close file */
    close(test_fd);

    return 0;
}
