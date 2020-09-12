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

void linked_list_free(linked_list *list) {
    while (list) {
        linked_list *to_free = list;
        list = list->next;
        my_free(to_free);
    }
}

linked_list* create_path_list(char *input, char separator) {
    linked_list *head = linked_list_create(file_path_create(input));
    linked_list *last = head;

    while (input[0] != '\0') {
        if (input[0] == separator) {
            input[0] = '\0';
            last = linked_list_push(last, file_path_create(input+1));
        }

        input += 1;
    }

    return head;
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

char* build_command(yamenu_app *app, file_path *path) {
    char *to_exec = my_malloc(strlen(app->prefix) + strlen(path->path) + strlen(app->postfix) + 5);
    sprintf(to_exec, "%s %s %s", app->prefix, path->path, app->postfix);
    return to_exec;
}

char* basefilename(const char *filename) {
    if (!filename) {
        return NULL;
    }
    char *s = strstr_last(filename, ".");
    if (strcmp(filename, ".") == 0
            || strcmp(filename, "..") == 0
            || filename[0] == '.') {
        return strdup(filename);
    } else if (!s) {
        return strdup(filename);
    } else {
        // only copy until '.'
        return strndup(filename, s-filename);
    }
}

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
