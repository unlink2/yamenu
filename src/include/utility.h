#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "data.h"

struct yamenu_app parse_args(int argc, char **argv);
void yamenu_app_init_paths(yamenu_app *app);

// #define DEBUG

// assert macro
#ifndef DEBUG
#define my_assert(x)
#else
#define my_assert(x) assert(x)
#endif

void* my_malloc(size_t);
void* my_realloc(void *, size_t);
void my_free(void *);

/**
 * Replaces a key with a new value.
 * returns:
 *  NULL if the key is not contained in the string
 *  a new pointer to a copy of the string with the new value inseted in place of key
 */
char *str_replace(const char *str, const char *key, const char *value);

#endif
