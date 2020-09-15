#include "include/utility.h"
#include "include/path.h"
#include "include/sysio.h"
#include <limits.h>

file_path* file_path_create(char *path, bool no_desktop_entry, read_file_source _read_file) {
    file_path *fp = my_malloc(sizeof(file_path));
    fp->path = path;


    fp->executable = NULL;
    fp->icon = NULL;
    fp->type = APPLICATION;
    fp->terminal = false;
    fp->name = NULL;
    fp->no_show = false;

#ifdef YAMENU_PARSE_DESKTOP_ENTRY
    if (_read_file) {
        // TODO remove this from path_create
        char *ext = fileext(path);
        if (ext) {
            if (strcmp(ext, ".desktop") == 0 && !no_desktop_entry) {
                // special case for .desktop files. parse them as ini
                // first read the file
                // now read file
                linked_list *list = _read_file(path);
                if (list) {
                    fp->executable = parse_desktop_entry("Exec=", list);
                    fp->name = parse_desktop_entry("Name=", list);
                    char *no_show = parse_desktop_entry("NoDisplay=", list);
                    if (no_show) {
                        fp->no_show = (strcmp(no_show, "true") == 0);
                        my_free(no_show);
                    }

                    for (size_t i = 0; i < linked_list_size(list); i++) {
                        my_free(linked_list_get(list, i)->generic);
                    }
                    linked_list_free(list);
                }

            }
            my_free(ext);
        }
    }
#endif

    return fp;
}

// helper function for exec var replacement
char *replace_all_exec_vars(char *str, const char *key, const char *value) {
    char *next = str;
    while (next) {
        next = str_replace(str, key, value);
        if (next) {
            my_free(str);
            str = next;
        }
    }
    return str;
}

char* parse_desktop_entry(char *key, linked_list *entry) {
    // gtk exec variables. we ignore all of them for this usecase
    const size_t exec_vars_len = 11;
    const char *exec_vars[] = {
        "%%",
        "%f",
        "%F",
        "%u",
        "%U",
        "%d",
        "%D",
        "%n",
        "%N",
        "%k",
        "%v"
    };
    const char *exec_vals[] = {
        "%",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
    };

    const char comment = '#';
    size_t first_line = 0;
    size_t entryc = linked_list_size(entry);

    if (entryc == 0) {
        return NULL;
    }
    // first line needs to be [Desktop Entry]

    while (linked_list_get(entry, first_line)->str[0] == comment
            || strlen(linked_list_get(entry, first_line)->str) == 0) {
        first_line++;
    }

    if (strcmp(linked_list_get(entry, first_line)->generic, "[Desktop Entry]") != 0) {
        return NULL; // not a desktop entry
    }
    
    first_line++;
    for (size_t i = first_line; i < entryc; i++) {
        size_t keylen = strlen(key);
        char *entry_iter = linked_list_get(entry, i)->generic;
        if (strncmp(key, entry_iter, keylen) == 0) {
            // TODO this is kinda awful.
            char *newval = strdup(entry_iter+keylen);
            for (size_t i = 0; i < exec_vars_len; i++) {
                newval = replace_all_exec_vars(newval, exec_vars[i], exec_vals[i]);
            }
            return newval;
        }
    }
    return NULL;
}

void file_path_free(file_path *fp) {
    if (fp->executable) {
        my_free(fp->executable);
    }
    if (fp->name) {
        my_free(fp->name);
    }
    if (fp->icon) {
        my_free(fp->icon);
    }
    my_free(fp);
}


