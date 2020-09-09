#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdbool.h>
#include "data.h"

typedef enum {
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERRROR,
    LEVEL_CRITICAL,
    LEVEL_FATAL,
} log_level;

bool should_log(log_level level, log_level max);

void yalogger(yamenu_app *app, log_level level, char *message);

#endif
