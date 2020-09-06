#ifndef __DATA_H__
#define __DATA_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "path.h"

// default value for nox flag
#define YAMENU_NOX_DEFAULT false

/**
 * Linked list of generic objects
 */
typedef struct linked_list {
    union {
        void *generic;
        char *str;
        file_path *fp;
    };
    struct linked_list *next;
} linked_list;

/**
 * Application settings object
 */
typedef struct yamenu_app {
    linked_list *path_list;
    char *shell;
    bool nox;
    char separator;
    char *input_list; // input for path list parser
} yamenu_app;

void yamenu_app_free(struct yamenu_app *app);

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

/**
 * input:
 *  An input string to be parsed.
 *  The input string is modified.
 * separator:
 *  The separator character
 * Returns:
 *  A linked list of paths
 */
linked_list* create_path_list(char *input, char separator);

/**
 * list:
 *  The path list
 * search:
 *  The search term
 * returns:
 *  A new linked list with paths containing the search term
 */
linked_list* filter_path_list(linked_list *list, char *search);

/**
 * executes a path using the application's settings
 * returns:
 *  The pid of the newly started process or 0 on error
 */
void execute_path(yamenu_app *app, file_path *path);

#endif
