#include "include/commandline.h"

#include <stdio.h>
#include <stdlib.h>

#define INPUT_BUFFER_SIZE 128

void command_line_interface(yamenu_app *app) {
    if (!app->path_list) {
        fprintf(stderr, "No input paths\n");
    }

    for (size_t i = 0; i < linked_list_size(app->path_list); i++) {
        printf("%ld) %s\n", i+1, linked_list_get(app->path_list, i)->fp->path);
    }

    char input_buffer[INPUT_BUFFER_SIZE];
    fgets(input_buffer, INPUT_BUFFER_SIZE, stdin);
}
