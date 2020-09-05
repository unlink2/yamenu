#ifndef __DATA_H__
#define __DATA_H__

#include <stdio.h>
#include <stdlib.h>

typedef char* path;

/**
 * Linked list of generic objects
 */
typedef struct linked_list {
    union {
        void *generic;
        path path;
    };
    struct linked_list *next;
} linked_list;

/**
 * Creates a new path list with
 * an initial value
 */
linked_list* linked_list_create(void *initial);

/**
 * Returns:
 *  A specific path from the path list
 *  NULL if the item is out of bounds
 */
linked_list* linked_list_get(linked_list *list, size_t index);

/**
 * Appends a new path to the end of the path list
 * Returns:
 *  A pointer to the newly created object
 */
linked_list* linked_list_push(linked_list *list, void *newobj);

/**
 * Pops the last item from a list
 * Returns:
 *  A pointer to the last item
 */
linked_list* linked_list_pop(linked_list *list);

/**
 * Returns:
 *  size of a list
 */
size_t linked_list_size(linked_list *list);

/**
 * Frees all linked_list objects in a list.
 * Does not fre the actual path strings.
 */
void linked_list_free(linked_list *list);

#endif
