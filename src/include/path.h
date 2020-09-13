#ifndef __PATH_H__
#define __PATH_H__

// enable parsing of .desktop files
// when creating a path object
#define YAMENU_PARSE_DESKTOP_ENTRY

typedef struct linked_list linked_list;

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

/**
 * path:
 *  Either the full path or just the executables name
 * pwd:
 *  If pwd is provided path must be just the file name
 *  in this case .desktop files are parsed in a special way
 */ 
file_path* file_path_create(char *path);

/**
 * Parses a .desktop file
 * Searched for a specific key, replaces (or removes) all exec vars
 * and returns a newly allocated string containing the requested value
 * key:
 *  the key
 * entry:
 *  linked list of strings containing the .desktop entry
 *  first item must be '[Desktop Entry]'
 */
char* parse_desktop_entry(char *key, linked_list *entry);

void file_path_free(file_path *fp);

#define file_path_get_real_name(fp) (fp->name ? fp->name : fp->path)

#endif

