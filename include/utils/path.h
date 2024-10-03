#ifndef MPGUI_PATH_H
#define MPGUI_PATH_H

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define handle_error(ERROR, FORMAT, ...)                                                                                                                       \
    if (ERROR) {                                                                                                                                               \
        fprintf(stderr, "%s -> %s -> %i -> Error(%i):\n\t%s\n\t" FORMAT "\n", __FILE_NAME__, __FUNCTION__, __LINE__, errno, strerror(errno), ##__VA_ARGS__);   \
        exit(EXIT_FAILURE);                                                                                                                                    \
    }

char* get_path_from_fd(int fd);

#endif //MPGUI_PATH_H
