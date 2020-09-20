#include "include/logger.h"

#include <stdio.h>
#include <stdarg.h>

bool should_log(log_level level, log_level max) {
    return max <= level;
}

void yalogger(yamenu_app *app, log_level level, char *message) {
    yalogger_var(app, level, message, NULL);
}

void yalogger_var(yamenu_app *app, log_level level, char *message, ...) {
    if (should_log(level, app->log_level)) {
        fprintf(stderr, "[%d] %s", level, message);
        va_list valist;
        va_start(valist, message);

        char *next_msg = NULL;
        while ((next_msg = va_arg(valist, char*)) != NULL) {
            printf("%s", next_msg);
        }

        va_end(valist);

        fprintf(stderr, "\n");
    }
}
