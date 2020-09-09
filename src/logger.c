#include "include/logger.h"

#include <stdio.h>

bool should_log(log_level level, log_level max) {
    return max <= level;
}

void yalogger(yamenu_app *app, log_level level, char *message) {
    if (should_log(level, app->log_level)) {
        fprintf(stderr, "[%d] %s\n", level, message);
    }
}
