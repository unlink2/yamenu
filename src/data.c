#include "include/data.h"
#include "include/utility.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "include/logger.h"

void yamenu_app_free(struct yamenu_app *app) {
    if (app->path_list) {
        for (size_t i = 0; i < linked_list_size(app->path_list); i++) {
            file_path_free(linked_list_get(app->path_list, i)->fp);
        }
        linked_list_free(app->path_list);
    }
}

linked_list* linked_list_create(void *initial) {
    linked_list *new_list = my_malloc(sizeof(linked_list));
    if (!new_list) {
        return NULL;
    }
    new_list->next = NULL;
    new_list->generic = initial;
    return new_list;
}

linked_list* linked_list_get(linked_list *list, size_t index) {
    for (size_t i = 0; i < index && list; i++) {
        list = list->next;
    }

    return list;
}

linked_list* linked_list_push(linked_list *list, void *newobj) {
    // find last object
    while (list->next) {
        list = list->next;
    }
    linked_list *new_item = linked_list_create(newobj);
    list->next = new_item;
    return new_item;
}

linked_list* linked_list_pop(linked_list *list) {
    if (!list) {
        return NULL;
    } else if (!list->next) {
        return list;
    }
    linked_list *previous;
    while (list->next) {
        previous = list;
        list = list->next;
    }
    linked_list *pop = list;
    previous->next = NULL;
    return pop;
}

size_t linked_list_size(linked_list *list) {
    size_t count = 0;
    while (list) {
        count++;
        list = list->next;
    }
    return count;
}

bool linked_list_swap(linked_list *list, size_t index1, size_t index2) {
    linked_list *l1 = linked_list_get(list, index1);
    linked_list *l2 = linked_list_get(list, index2);

    if (!l1 || !l2) {
        return false;
    }

    void *temp = l1->generic;
    l1->generic = l2->generic;
    l2->generic = temp;

    return true;
}

size_t linked_list_pivot(linked_list *list, size_t low, size_t high, linked_list_compare compare) {
    linked_list *piv = linked_list_get(list, high);
    size_t index = (low - 1);

    for (size_t i = low; i < high; i++) {
        linked_list *current = linked_list_get(list, i);
        if (compare(piv, current) >= 0) {
            index++;
            linked_list_swap(list, index, i);
        }
    }
    linked_list_swap(list, index+1, high);

    return index+1;
}

void linked_list_quick_sort(linked_list *list, size_t low, size_t high, linked_list_compare compare) {
    if (high == 0) {
        return;
    }
    if (low < high) {
        int pi = linked_list_pivot(list, low, high, compare);
        // prevent underflwo if high
        if (pi == 0) {
            return;
        }
        linked_list_quick_sort(list, low, pi-1, compare);
        linked_list_quick_sort(list, pi, high, compare);
    }
}

linked_list* linked_list_cat(linked_list *l1, linked_list *l2) {
    linked_list *head = l1;
    while (l1->next) {
        l1 = l1->next;
    }
    l1->next = l2;
    return head;
}

void linked_list_free(linked_list *list) {
    while (list) {
        linked_list *to_free = list;
        list = list->next;
        my_free(to_free);
    }
}

void shift_str_left(char *input) {
    int n = strlen(input);
    for (size_t i = 0; i < n-1; i++) {
        input[i] = input[i+1];
    }
    input[n-1] = '\0';
}

void unescape_str(char *input) {
    int n = strlen(input);
    for (size_t i = 0; i < strlen(input); i++) {
        if (input[i] == '"') {
            // shift 1 left
            shift_str_left(input+i);
            n--;
        } else if (input[i] == '\\') {
            // shift 1 left
            shift_str_left(input+i);
            n--;
        }
    }
}

linked_list* create_path_list(char *input, char separator, bool no_desktop_entry, read_file_source _read_file) {
    linked_list *head = linked_list_create(file_path_create(input, no_desktop_entry, _read_file));
    linked_list *last = head;

    const char quote_char = '"';
    const char escape_char = '\\';

    bool quoted = false;
    bool escaped = false;

    while (input[0] != '\0') {
        if (input[0] == separator && !quoted && !escaped) {
            input[0] = '\0';
            // unescape the last input
            unescape_str(last->fp->path);
            last = linked_list_push(last, file_path_create(input+1, no_desktop_entry, _read_file));
        } else if (input[0] == quote_char && !escaped) {
            quoted = !quoted;
        } else if (input[0] == escape_char) {
            escaped = true;
        } else {
            escaped = false;
        }

        input += 1;
    }
    unescape_str(last->fp->path);
    return head;
}

// helper function
bool is_excluded(char *path, linked_list *excludes) {
    for (size_t i = 0; i < linked_list_size(excludes); i++) {
        linked_list *current = linked_list_get(excludes, i);
        if (strcmp(path, current->fp->path) == 0) {
            return true;
        }
    }

    return false;
}

linked_list* apply_exclude_list(linked_list *path_list, char *excludes, char separator) {
    if (!excludes) {
        return path_list;
    }
    linked_list *exclude_list = create_path_list(excludes, separator, true, NULL);
    linked_list *new_list = NULL;
    for (size_t i = 0; i < linked_list_size(path_list); i++) {
        linked_list *current = linked_list_get(path_list, i);
        if (!is_excluded(current->fp->path, exclude_list)) {
            if (!new_list) {
                new_list = linked_list_create(current->fp);
            } else {
                linked_list_push(new_list, current->fp);
            }
        } else {
            file_path_free(current->fp);
        }
    }

    for (size_t i = 0; i < linked_list_size(exclude_list); i++) {
        file_path_free(linked_list_get(exclude_list, i)->fp);
    }

    linked_list_free(exclude_list);
    linked_list_free(path_list);

    return new_list;
}

linked_list* filter_path_list(linked_list *list, char *search) {
    linked_list *head = NULL;
    linked_list *last = NULL;


    // search the whole list
    // assume that the list only contains paths!
    while (list) {
        if (!strstr(list->fp->path, search)) {
            list = linked_list_get(list, 1);
            continue;
        }
        if (!head) {
            head = linked_list_create(list->fp);
            last = head;
        } else {
            last = linked_list_push(last, list->fp);
        }
        list = linked_list_get(list, 1);
    }

    return head;
}

int path_list_compare(linked_list *l1, linked_list *l2) {
    return string_sort_helper(file_path_get_real_name(l1->fp), file_path_get_real_name(l2->fp));
}

int string_sort_helper(char *l1, char *l2) {
    size_t len1 = strlen(l1);
    size_t len2 = strlen(l2);
    size_t min_len = MIN(len1, len2);

    for (size_t i = 0; i < min_len; i++) {
        if (l1[i] > l2[i]) {
            return 1; // greater
        } else if (l1[i] < l2[i]) {
            return -1; // smaller
        }
    }

    return len2-len1; // equal, but different lenght?
}

char* build_command(yamenu_app *app, file_path *path) {
    char *to_exec = my_malloc(strlen(app->prefix) + strlen(path->path) + strlen(app->postfix) + 5);

    char *executable = path->path;
    if (path->executable) {
        executable = path->executable;
    }

    sprintf(to_exec, "%s %s %s", app->prefix, executable, app->postfix);
    return to_exec;
}

char* basefilename(const char *filename) {
    if (!filename) {
        return NULL;
    }
    char *s = strstr_last(filename, ".");
    if (strcmp(filename, "..") == 0
            || filename[0] == '.') {
        return strdup(filename);
    } else if (!s) {
        return strdup(filename);
    } else {
        // only copy until '.'
        return strndup(filename, s-filename);
    }
}

char *fileext(const char *filename) {
    if (!filename) {
        return NULL;
    }
    char *s = strstr_last(filename, ".");
    if(strcmp(filename, "..") == 0
            || filename[0] == '.') {
        return strdup(filename);
    } else if (!s) {
        return NULL;
    } else {
        // only copy after '.'
        return strdup(s);
    }}

char* strstr_last(const char *str, const char *search) {
    char *current = NULL;
    char *tmp = (char*)str-1;
    char *last = NULL;

    while (tmp) {
        last = current;
        tmp = strstr(tmp+1, search);
        current = tmp;
    }
    return last;
}
