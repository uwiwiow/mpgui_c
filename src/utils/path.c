#include "utils/path.h"

char* get_path_from_fd(int fd) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);

    static char actual_path[PATH_MAX];
    ssize_t len = readlink(path, actual_path, sizeof(actual_path) - 2);
    handle_error(len == -1, "readlink fd:%d", fd);

    actual_path[len] = '\0';

    if (actual_path[len - 1] != '/') {
        actual_path[len] = '/';
        actual_path[len + 1] = '\0';
    }

    return actual_path;
}
