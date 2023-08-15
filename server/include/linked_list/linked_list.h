#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct llist llist_t;

llist_t *llist_init();
void llist_add(llist_t *list, void *data);
void *llist_pop(llist_t *list);
void *llist_get_data(llist_t *list, int index);
int llist_get_num_node(llist_t *list);
void llist_deinit(llist_t *list);

#endif
