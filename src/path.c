#include "include/utility.h"
#include "include/path.h"
#include "include/sysio.h"
#include <limits.h>

file_path* file_path_create(char *path, char *pwd) {
    file_path *fp = my_malloc(sizeof(file_path));
    fp->path = path;


    fp->executable = NULL;
    fp->icon = NULL;
    fp->type = APPLICATION;
    fp->terminal = false;
    fp->name = NULL;

#ifdef YAMENU_PARSE_DESKTOP_ENTRY
    char *ext = fileext(path);
    if (ext) {
        if (strcmp(ext, ".desktop") == 0) {
            // special case for .desktop files. parse them as ini
            // first read the file

            // if a pwd is provided combine with path, otherwise just read path
            char cwd[PATH_MAX];
            getcwd(cwd, PATH_MAX);
            if (pwd) {
                // chdir
                chdir(pwd);
            }
            // now read file
            linked_list *list = read_file(path);
            if (list) {
                fp->executable = parse_desktop_entry("Exec=", list);
                fp->name = parse_desktop_entry("Name=", list);

                for (size_t i = 0; i < linked_list_size(list); i++) {
                    my_free(linked_list_get(list, i)->generic);
                }
                linked_list_free(list);
            }
            chdir(cwd); // change path back

        }
        my_free(ext);
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

    size_t entryc = linked_list_size(entry);

    if (entryc == 0) {
        return NULL;
    }
    // first line needs to be [Desktop Entry]

    if (strcmp(linked_list_get(entry, 0)->generic, "[Desktop Entry]") != 0) {
        return NULL; // not a desktop entry
    }

    for (size_t i = 1; i < entryc; i++) {
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


