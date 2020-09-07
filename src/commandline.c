#include "include/commandline.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 128

void command_line_interface(yamenu_app *app) {
    if (!app->path_list) {
        fprintf(stderr, "No input paths\n");
        return;
    }

    char input_buffer[INPUT_BUFFER_SIZE];
    fgets(input_buffer, INPUT_BUFFER_SIZE, stdin);
    // removed \n from input
    input_buffer[strlen(input_buffer)-1] = '\0';

    linked_list *filtered = filter_path_list(app->path_list, input_buffer);

    for (size_t i = 0; i < linked_list_size(filtered); i++) {
        printf("%ld) %s\n", i+1, linked_list_get(filtered, i)->fp->path);
    }

    fgets(input_buffer, INPUT_BUFFER_SIZE, stdin);

    size_t selection = atoi(input_buffer);
    linked_list *path_list = linked_list_get(filtered, selection-1);

    if (path_list) {
        execute_path(app, path_list->fp);
    }

    linked_list_free(filtered);
}
