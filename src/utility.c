#include <argp.h>
#include <stdio.h>
#include <string.h>

#include "include/logger.h"
#include "include/utility.h"


const char *argp_program_version = "yamenu 0.0.1";
const char *argp_program_bug_address = "<lukas@krickl.dev>";
static char doc[] = "Yet another X launcher.";
static char args_doc[] = "";
static struct argp_option options[] = {
    { "separator", 's', "char", OPTION_ARG_OPTIONAL, "Specify list separator. Defaults to ';'"},
    { "paths", 'p', "paths", OPTION_ARG_OPTIONAL, "<separator> terminated list of paths."},
    { "nox", 'n', 0, OPTION_ARG_OPTIONAL, "Run command line mode."},
    { "prefix", 'P', "prefix", OPTION_ARG_OPTIONAL, "Added before every command."},
    { "postfix", 'F', "postfix", OPTION_ARG_OPTIONAL, "Added after every command."},
    { "verbose", 'v', NULL, OPTION_ARG_OPTIONAL, "Enables verbose logging."},
    { 0 }
};



static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct yamenu_app *arguments = state->input;
    switch (key) {
        case 'n':
            arguments->nox = true;
            break;
        case 'p':
            arguments->input_list = arg;
            break;
        case 's':
            if (arg && strlen(arg) == 1) {
                arguments->separator = arg[0];
            }
            break;
        case 'P':
            arguments->prefix = arg;
            break;
        case 'F':
            arguments->postfix = arg;
            break;
        case 'v':
            arguments->log_level = LEVEL_DEBUG;
            break;
        case ARGP_KEY_ARG:
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {
    options,
    parse_opt,
    args_doc,
    doc,
    0, 0, 0};

struct yamenu_app parse_args(int argc, char **argv) {
    struct yamenu_app arguments;

    arguments.nox = YAMENU_NOX_DEFAULT;
    arguments.separator = ';';
    arguments.input_list = NULL;
    arguments.path_list = NULL;
    arguments.shell = "/bin/sh";
    arguments.prefix = "";
    arguments.postfix = "";
    arguments.log_level = LEVEL_ERRROR;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // NULL check is required because
    // an empty input may yield NULL
    if (arguments.input_list) {
        arguments.path_list = create_path_list(arguments.input_list, arguments.separator);
    }

    return arguments;
}

void* my_malloc(size_t size) {
    void *ptr = malloc(size);
    /*if (ptr == NULL) {
        exit(-1);
    }*/
    my_assert(ptr != NULL);

    memset(ptr, 0, size);

    return ptr;
}

void* my_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

void my_free(void *ptr) {
    // run assert on a pointer that is already marked as NULL
    my_assert(ptr != NULL);
    if (ptr != NULL) {
        free(ptr);
    }
}
