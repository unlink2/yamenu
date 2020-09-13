#include "include/utility.h"
#include "include/path.h"

file_path* file_path_create(char *path) {
    file_path *fp = my_malloc(sizeof(file_path));
    fp->path = path;


    fp->executable = NULL;
    fp->icon = NULL;
    fp->type = APPLICATION;
    fp->terminal = false;
    fp->name = NULL;

#ifdef YAMEU_PARSE_DESKTOP_ENTRY
    char *ext = fileext(path);
    if (ext) {
        if (strcmp(ext, ".desktop") == 0) {
            // special case for .desktop files. parse them as ini
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

char* parse_desktop_entry(char *key, char **entry, size_t entryc) {
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

    if (entryc == 0) {
        return NULL;
    }
    // first line needs to be [Desktop Entry]

    if (strcmp(entry[0], "[Desktop Entry]") != 0) {
        return NULL; // not a desktop entry
    }

    for (size_t i = 1; i < entryc; i++) {
        size_t keylen = strlen(key);
        if (strncmp(key, entry[i], keylen) == 0) {
            // TODO this is kinda awful.
            char *newval = strdup(entry[i]+keylen);
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
