#ifndef __PATH_H__
#define __PATH_H__

typedef struct file_path {
    char *path;
} file_path;

file_path* file_path_create(char *path);

void file_path_free(file_path *fp);

#endif

