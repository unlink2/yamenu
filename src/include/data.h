#ifndef __DATA_H__
#define __DATA_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iniparser.h>

#include "path.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

// default value for nox flag
#define YAMENU_NOX_DEFAULT false

// default location of desktop applications
#define YAMENU_DEFAULT_SEARCH_PATH "/usr/share/applications"


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
 * Comparison function
 * returns:
 *  0 if equal
 *  < 0 if 2nd item is greater
 *  > 0 if 1st item is greater
 */
typedef int (*linked_list_compare)(linked_list *, linked_list *);

/**
 * Points to a function that reads a file from memory and
 * returns:
 *  a linked list of lines
 *  NULL on error
 */
typedef linked_list* (*read_file_source)(char *path);

#define DEFAULT_X_Y_POS -1

/**
 * Application settings object
 */
typedef struct yamenu_app {
    linked_list *path_list;
    char *shell;
    bool nox;
    char separator;
    char *input_list; // input for path list parser
    char *prefix;
    char *postfix;
    short log_level;

    char *excludes;
    char *search_path;
    bool show_hidden;
    bool base_name_only;
    bool dry_run;
    bool no_desktop_entry;

    char *cfg_ext; //defaults to .desktop

    char *term;

    // if position of both is -1 winodw will default to center of screen
    int x_pos;
    int y_pos;

    read_file_source _read_file;
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
 * Swaps values between index1 and index2
 */
bool linked_list_swap(linked_list *list, size_t index1, size_t index2);

/**
 * Quicksort for linked lists
 * list:
 *  The list
 * compare:
 *  a comparison function for the list items
 */
void linked_list_quick_sort(linked_list *list, size_t low, size_t high, linked_list_compare compare);

// TODO implement quicksort for linked list. This requires a way to swap values for linekd list.
// it should only swap the generic void ptr and not the acual list entries. This preserves the head

/**
 * Concatinates two linked lists
 */
linked_list* linked_list_cat(linked_list *l1, linked_list *l2);

/**
 * Frees all linked_list objects in a list.
 * Does not fre the actual path strings.
 */
void linked_list_free(linked_list *list);

// shifts input string 1 to the left
void shift_str_left(char *input);
// removes escape sequences from input string
void unescape_str(char *input);

/**
 * input:
 *  An input string to be parsed.
 *  The input string is modified.
 * separator:
 *  The separator character
 * no_desktop_entry:
 *  disables the automatic parsing of .desktop files
 * Returns:
 *  A linked list of paths
 */
linked_list* create_path_list(char *input, char separator, bool no_desktop_entry,
        read_file_source _read_file, char *cfg_ext);

/**
 * Applies an exclude list to a path list. Frees items that are not filtered
 * path_list:
 *  the path linked list. Items on the exclude list will be removed and freed
 * excludes:
 *  A list of exclude paths. will be parsed by create_path_list
 * returns:
 *  a new linked list that applied the exclude list
 */
linked_list* apply_exclude_list(linked_list *path_list, char *excludes, char separator);

/**
 * list:
 *  The path list
 * search:
 *  The search term
 * returns:
 *  A new linked list with paths containing the search term
 */
linked_list* filter_path_list(linked_list *list, char *search);

int path_list_compare(linked_list *l1, linked_list *l2);
int string_sort_helper(char *l1, char *l2);

/**
 * Builds a command based on the global application settings
 * and the current path
 */
char* build_command(yamenu_app *app, file_path *path);

/**
 * Extracts the base name from a given filename (anything after '.' is removed)
 * Note that it only takes the first occurance of '.' into consideration.
 * returns:
 *  a newly allocated string containing the result
 *  it always returns '.' and '..' as is
 *  returns hidden files as is (files starting with '.')
 */
char* basefilename(const char *filename);
/**
 * Oposite of basefilename
 * returns:
 *  the extension
 *  always returns '.' and '..' as is
 */
char *fileext(const char *filename);

/**
 * Works like strstr but finds the last occurange of string
 */
char* strstr_last(const char *str, const char *search);


#endif
