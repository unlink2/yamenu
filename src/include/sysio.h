#ifndef __SYSIO_H__
#define __SYSIO_H__

#include "data.h"
#include "path.h"
#include <stdio.h>

#define PATH_SEP '/'

typedef char* (path_filter_fn)(const char *);

/**
 * Reads a directory and returns a path list
 * path:
 *  The path
 * all:
 *  include hidden files
 * only_files:
 *  do not include anything but regular files
 * filter:
 *  Function pointer to a function that may filter the input.
 *  If NULL is provided strdup is used
 */
linked_list* create_path_list_from_dir(char *path, bool all, bool only_files, path_filter_fn *filter);


/**
 * executes a path using the application's settings
 * returns:
 *  The pid of the newly started process or 0 on error
 */
void execute_path(yamenu_app *app, file_path *path);

/**
 * Reads a file linke by line into a linked list
 * removes new line character from end of string
 * returns:
 *  a linked list of char*
 *  NULL on error
 */
linked_list* read_file(char *path);

#endif
