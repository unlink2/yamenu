#include "include/data.h"
#include "include/utility.h"
#include <string.h>

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

void linked_list_free(linked_list *list) {
    while (list) {
        linked_list *to_free = list;
        list = list->next;
        my_free(to_free);
    }
}

linked_list* create_path_list(char *input, char separator) {
    linked_list *head = linked_list_create(input);
    linked_list *last = head;

    while (input[0] != '\0') {
        if (input[0] == separator) {
            input[0] = '\0';
            last = linked_list_push(last, input+1);
        }

        input += 1;
    }

    return head;
}
