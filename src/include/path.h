#ifndef __PATH_H__
#define __PATH_H__

// enable parsing of .desktop files
// when creating a path object
#define YAMENU_PARSE_DESKTOP_ENTRY

typedef enum {
    APPLICATION
} path_type;

typedef struct file_path {
    char *path;
    char *name;
    char *executable;
    bool terminal;
    char *icon;
    path_type type;
} file_path;

file_path* file_path_create(char *path);

/**
 * Parses a .desktop file
 * Searched for a specific key, replaces (or removes) all exec vars
 * and returns a newly allocated string containing the requested value
 * key:
 *  the key
 * entry:
 *  array of strings without \n
 *  first item must be '[Desktop Entry]'
 * entryc:
 *  amount of strings in array
 */
char* parse_desktop_entry(char *key, char **entry, size_t entryc);

void file_path_free(file_path *fp);

#endif

