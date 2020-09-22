#include "include/sysio.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "include/data.h"
#include "include/utility.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "include/logger.h"

linked_list* create_path_list_from_dir(char *path, bool all, bool only_files, path_filter_fn *filter,
        bool no_desktop_entry, read_file_source _read_file, char *cfg_ext) {
    if (!filter) {
        filter = strdup;
    }

    DIR *directory = opendir(path);

    if (!directory) {
        return NULL;
    }

    linked_list *response = NULL;
    linked_list *last = response;
    struct dirent *de;
    while ((de = readdir(directory)) != NULL) {
        if (de->d_type != DT_REG && only_files) {
            continue;
        }

        // is it hidden?
        if (!all && de->d_name && de->d_name[0] == '.') {
            continue;
        }

        // apply the filter
        char *filtered = filter(de->d_name);
        if (!filtered) {
            continue;
        }

        // combine path with file name
        char *full = path_combine(path, filtered, PATH_SEP);
        my_free(filtered);
        if (!response) {
            response = linked_list_create(file_path_create(full, no_desktop_entry, _read_file, cfg_ext));
            last = response;
        } else {
            last = linked_list_push(last, file_path_create(full, no_desktop_entry, _read_file, cfg_ext));
        }
    }

    closedir(directory);

    return response;
}

void execute_path(yamenu_app *app, file_path *path) {
    if (app->dry_run) {
        // if it is a dry run just make the command  print it and return
        char *to_exec = build_command(app, path);
        yalogger(app, LEVEL_CRITICAL, to_exec);
        my_free(to_exec);
        return;
    }

    // TODO double fork to get pid 1 as parent and close all open files
    // and re-open stdin, stdout and stderr

    if (!path) {
        return;
    }

    // double fork to prevent zombie
    /*int pid_single = fork();
    if (pid_single > 0) {
        // parent
        return;
    } else if (pid_single < 0) {
        // TODO error!
        return;
    }
    // pid is 0 fork was successfull
    int pid_double = fork();
    if (pid_double != 0) {
        // child 1
        exit(pid_double > 0);
    }

    // pid is 0 again fork was successfull
    setsid(); // get new session*/

    struct passwd *pw = getpwuid(getuid());
    chdir(pw->pw_dir);

    // this will leak, but we're also going to exit right after. not so bad
    char *to_exec = build_command(app, path);
    yalogger(app, LEVEL_INFO, to_exec);
    execl(app->shell, app->shell, "-c", to_exec, NULL);

    // should not get here
    yalogger_var(app, LEVEL_FATAL, "Could not launch application: ", to_exec, NULL);
    die();
}

linked_list* read_file(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return NULL;
    }

    char *line = NULL;
    size_t read;
    size_t len;

    linked_list *result = NULL;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line[strlen(line)-1] == '\n') {
            line[strlen(line)-1] = '\0';
        }
        if (result) {
            linked_list_push(result, strdup(line));
        } else {
            result = linked_list_create(strdup(line));
        }
    }

    return result;
}


// TODO test this function
void yamenu_app_init_paths(yamenu_app *app) {
    // NULL check is required because
    // an empty input may yield NULL
    if (app->input_list) {
        app->path_list = create_path_list(app->input_list, app->separator, app->no_desktop_entry,
                app->_read_file, app->cfg_ext);
        app->path_list = apply_exclude_list(app->path_list, app->excludes, app->separator);
    } else if (app->search_path) {
        // if an empty list was provided list the search directory instead

        // TODO unit test this
        linked_list *search_paths = create_path_list(app->search_path, app->separator,
                app->no_desktop_entry, app->_read_file, app->cfg_ext);
        while (search_paths) {
            linked_list *next = create_path_list_from_dir(search_paths->fp->path, app->show_hidden,
                true, app->base_name_only ? basefilename : NULL, app->no_desktop_entry, app->_read_file, app->cfg_ext);
            if (!app->path_list) {
                app->path_list = next;
            } else {
                linked_list_cat(app->path_list, next);
            }

            search_paths = search_paths->next;
        }
        if (app->path_list) {
            app->path_list = apply_exclude_list(app->path_list, app->excludes, app->separator);
            linked_list_quick_sort(app->path_list, 0, linked_list_size(app->path_list)-1, path_list_compare);
        }
    } else {
        die();
    }
}

void die() {
    exit(-1);
}
