#include "linked_list.h"
#include <stdlib.h>

typedef struct node
{
    void* data;
    struct node *next;
} node_t;
 
struct llist
{
    int size;
    node_t *head;
};
 
llist_t *llist_init()
{
    llist_t *new_list = (llist_t *)malloc(sizeof(llist_t));
    new_list->size = 0;
    new_list->head = NULL;
    return new_list;
}
 
void llist_add(llist_t *list, void *data)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->data = data;
    new_node->next = (struct node *)list->head;
    list->head = new_node;
    list->size++;
}
 
void *llist_pop(llist_t *list)
{
    if (list->size == 0) {
        return NULL;
    }
    node_t *node_to_remove = list->head;
    void *data = node_to_remove->data;
    list->head = (node_t *)node_to_remove->next;
    free(node_to_remove);
    list->size--;
    return data;
}

void *llist_get_data(llist_t *list, int index)
{
    if(index < list->size)
    {
        node_t *temp_node = list->head;
        for(int i = 0; i < (list->size - index - 1); i++)
        {
            temp_node = (node_t *)temp_node->next;
        }
        return temp_node->data;
    }
    else
    {
        return NULL;
    }
}

int llist_get_num_node(llist_t *list)
{
    return list->size;
}
 
void llist_deinit(llist_t *list)
{
    node_t *current_node = list->head;
    while (current_node != NULL) {
        node_t *next_node = (node_t *)current_node->next;
        free(current_node);
        current_node = next_node;
    }
    free(list);
}