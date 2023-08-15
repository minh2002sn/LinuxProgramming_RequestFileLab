#include "file_req_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "command_header.h"
#include "linked_list.h"
#include "app_config.h"
#include "error_checker.h"

static llist_t *g_file_list = NULL;
static uint16_t total_file_name_len = 0;

static void file_list_update()
{
    DIR *dir;
    struct dirent *entry;
    if (g_file_list != NULL)
        return;
    

    /* Open directory */
    dir = opendir(DATA_FOLDER);
    if(dir)
    {
        // if(g_file_list == NULL)
        // {
        //     g_file_list = llist_init();
        // }
        // else
        // {

        // }
        
        g_file_list = llist_init();
        total_file_name_len = 0;
        while((entry = readdir(dir)) != NULL)
        {
            /* Remove ". and ".." directory */
            if(entry->d_name[0] != '.')
            {
                char *file_name = (char *)malloc(sizeof(entry->d_name));
                strcpy(file_name, entry->d_name);
                total_file_name_len += strlen(entry->d_name);
                llist_add(g_file_list, (void *)file_name);
                // printf("%s\n", (char *)entry->d_name);
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

void filelist_req_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    int ret = 0;
    cmdhdr_t cmd_header;

    /* Update list of files */
    file_list_update();

    /* Create header */
    cmd_header.header = CMD_HEADER;
    cmd_header.command = CMD_REP_FILE_LIST;
    cmd_header.data_len = total_file_name_len;

    /* Send header */
    ret = write(conn_fd, (uint8_t *)&cmd_header, sizeof(cmd_header));
    ERROR_CHECK(ret, "write()");

    /* Send list of files */
    for(int i = 0; i < llist_get_num_node(g_file_list); i++)
    {
        char *curr_file_name = (char *)llist_get_data(g_file_list, i);
        // printf("%s\n", curr_file_name);
        char tx_buff[255];
        sprintf(tx_buff, "%d_%s\n", i, curr_file_name);
        write(conn_fd, tx_buff, strlen(tx_buff));
    }
}

void file_req_handle(int conn_fd, uint8_t *buff, uint16_t size)
{
    if(size == 1 && (buff[0] >= 1 && buff[0] <= llist_get_num_node(g_file_list)))
    {
        int test_fd = 0;
        int ret = 0;
        struct stat st;
        cmdhdr_t cmd_header;
        char file_path[255];

        /* Get size of file */
        sprintf(file_path, "%s/%s", DATA_FOLDER, (char *)llist_get_data(g_file_list, buff[0]));
        ret = stat(file_path, &st);
        ERROR_CHECK(ret, "stat()");
        
        /* Create header */
        cmd_header.header = CMD_HEADER;
        cmd_header.command = CMD_REP_FILE;
        cmd_header.data_len = st.st_size;

        /* Open file */
        test_fd = open(file_path, O_RDONLY);
        ERROR_CHECK(test_fd, "open()");

        /* Send header */
        ret = write(conn_fd, (uint8_t *)&cmd_header, sizeof(cmd_header));
        ERROR_CHECK(ret, "write()");

        /* Send fie */
        ret = sendfile(conn_fd, test_fd, NULL, st.st_size);
        ERROR_CHECK(ret, "sendfile()");

        /* Close file */
        close(test_fd);
    }
}
