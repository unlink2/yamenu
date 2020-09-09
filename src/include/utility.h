#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "data.h"

struct yamenu_app parse_args(int argc, char **argv);

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

#endif
