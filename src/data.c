#include "include/data.h"
#include "include/utility.h"

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
