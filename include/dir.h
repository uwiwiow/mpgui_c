#ifndef MPGUI_DIR_H
#define MPGUI_DIR_H

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "song.h"

#define handle_error(ERROR, FORMAT, ...)                                                                                                                       \
    if (ERROR) {                                                                                                                                               \
        fprintf(stderr, "%s -> %s -> %i -> Error(%i):\n\t%s\n\t" FORMAT "\n", __FILE_NAME__, __FUNCTION__, __LINE__, errno, strerror(errno), ##__VA_ARGS__);   \
        exit(EXIT_FAILURE);                                                                                                                                    \
    }

char* get_dir_from_fd(int fd);

void dir_recurse (DIR *parent, int level);


#endif //MPGUI_DIR_H
