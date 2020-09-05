#include "include/utility.h"
#include "include/path.h"

file_path* file_path_create(char *path) {
    file_path *fp = my_malloc(sizeof(file_path));
    fp->path = path;
    return fp;
}

void file_path_free(file_path *fp) {
    my_free(fp);
}
