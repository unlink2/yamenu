#include <argp.h>
#include <stdio.h>
#include <string.h>

#include "include/logger.h"
#include "include/utility.h"
#include "include/sysio.h"

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
    { "search", 'S', "path", OPTION_ARG_OPTIONAL, "Lists a given directory. This only works if --paths is not provided."},
    { "all", 'a', NULL, OPTION_ARG_OPTIONAL, "Include hidden files in the list"},
    { "base", 'b', NULL, OPTION_ARG_OPTIONAL, "Return path's basename only (Removes extension)"},
    { "dry", 'D', NULL, OPTION_ARG_OPTIONAL, "Do not execute the command."},
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
        case 'a':
            arguments->show_hidden = true;
            break;
        case 'b':
            arguments->base_name_only = true;
            break;
        case 'S':
            arguments->search_path = arg;
            break;
        case 'D':
            arguments->dry_run = true;
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
    arguments.base_name_only = false;
    arguments.show_hidden = false;
    arguments.search_path = YAMENU_DEFAULT_SEARCH_PATH;
    arguments.dry_run = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    return arguments;
}

void yamenu_app_init_paths(yamenu_app *app) {
    // NULL check is required because
    // an empty input may yield NULL
    if (app->input_list) {
        app->path_list = create_path_list(app->input_list, app->separator);
    } else {
        // if an empty list was provided list the search directory instead

        // TODO unit test this
        linked_list *search_paths = create_path_list(app->search_path, app->separator);
        while (search_paths) {
            linked_list *next = create_path_list_from_dir(search_paths->fp->path, app->show_hidden,
                true, app->base_name_only ? basefilename : NULL);
            if (!app->path_list) {
                app->path_list = next;
            } else {
                linked_list_cat(app->path_list, next);
            }

            search_paths = search_paths->next;
        }
        linked_list_quick_sort(app->path_list, 0, linked_list_size(app->path_list)-1, path_list_compare);
    }
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

char *str_replace(const char *str, const char *key, const char *value) {
    // match for token
    char *match = strstr(str, key);

    if (!match) {
        return NULL;
    }

    // should be the exact lenght + 1 for \0 just in case
    int newlen = strlen(value) + strlen(str) - strlen(key) + 1;
    // now we can replace!
    // and by replace I mean create a new string
    // because there may not be enough space
    char *newstr = my_malloc(newlen * sizeof(char));

    // copy first bit of string until the end
    const int first_part = match - str;
    // skip the label to be replaced + 1 for space
    const int second_part = first_part + strlen(key);

    strncat(newstr, str, first_part);
    strncat(newstr, value, first_part + strlen(value));
    strncat(newstr, str + second_part, newlen);

    return newstr;
}

char* path_combine(char *base, char *append, char sep) {
    int baselen = strlen(base);
    int appendlen = strlen(append);
    if (base[baselen-1] == sep) {
        // remove separating char from end
        baselen--;
    }
    if (append[0] == sep) {
        append++;
        appendlen--;
    }
    // make new string
    char *newpath = my_malloc(baselen + appendlen + 2);
    sprintf(newpath, "%*.*s%c%*.*s", baselen, baselen, base, sep, appendlen, appendlen, append);
    return newpath;
}
