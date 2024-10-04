#include "dir.h"

char* get_dir_from_fd(int fd) {
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

void dir_recurse (DIR *parent, int level) {
    struct dirent *ent;
    handle_error(parent == NULL, "open dir")

    while ((ent = readdir(parent)) != NULL) {
        if ((strcmp(ent->d_name, ".") == 0) ||
            (strcmp(ent->d_name, "..") == 0)) {
            continue;
        }

        int parent_fd = dirfd(parent);
        handle_error(parent_fd == -1, "dir fd")

        int fd = openat(parent_fd, ent->d_name, O_RDONLY | O_DIRECTORY);

        if (fd != -1) {

            DIR *child = fdopendir(fd);
            handle_error(child == NULL, "fdopendir")

            dir_recurse(child, level + 1);
            closedir(child);

        } else if (errno == ENOTDIR) {
            song_metadata m;
            if (get_metadata(ent->d_name, get_dir_from_fd(parent_fd), &m) != -1) {
                printf("%s\t%s\t%s\t%s\t%s\t%d\t%d\n", m.title, m.artist, m.album, m.comment, m.genre, m.year, m.track);
                free_song_metadata(&m);
            }

        } else {
            handle_error(1, "openat")
        }
    }
}