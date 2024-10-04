#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <taglib/tag_c.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include "utils/path.h"

#define handle_error(ERROR, FORMAT, ...)                                                                                                                       \
    if (ERROR) {                                                                                                                                               \
        fprintf(stderr, "%s -> %s -> %i -> Error(%i):\n\t%s\n\t" FORMAT "\n", __FILE_NAME__, __FUNCTION__, __LINE__, errno, strerror(errno), ##__VA_ARGS__);   \
        exit(EXIT_FAILURE);                                                                                                                                    \
    }

typedef struct song_metadata {
    char* title;
    char* artist;
    char* album;
    char* comment;
    char* genre;
    unsigned int year;
    unsigned int track;
} song_metadata;

void free_song_metadata(song_metadata *m) {
    if (m->title) free(m->title);
    if (m->artist) free(m->artist);
    if (m->album) free(m->album);
    if (m->comment) free(m->comment);
    if (m->genre) free(m->genre);
}

int get_name(char* name, char* dirname, song_metadata* songMetadata) {
    size_t len_dirname = strlen(dirname);
    size_t len_name = strlen(name);
    char *fullpath = (char*)malloc(len_dirname + len_name + 2);
    handle_error(fullpath == NULL, "malloc");

    strcpy(fullpath, dirname);
    strcat(fullpath, name);

    TagLib_File *song = taglib_file_new(fullpath);
    handle_error(song == NULL, "taglib_file_new: %s", fullpath);

    TagLib_Tag *tag = taglib_file_tag(song);
    if (tag == NULL) {
        fprintf(stderr, "%s -> %s -> %i -> Error:\n\ttaglib_file_tag: %s\n", __FILE_NAME__, __FUNCTION__, __LINE__ - 2,  fullpath);
        taglib_file_free(song);
        free(fullpath);
        return -1;
    }

    char *title = taglib_tag_title(tag);
    songMetadata->title = (title && strlen(title) > 0) ? strdup(title) : strdup(name);
    handle_error(songMetadata->title == NULL, "strdup(title): %s", fullpath);

    char *artist = taglib_tag_artist(tag);
    songMetadata->artist = (artist && strlen(artist) > 0) ? strdup(artist) : strdup("Unknown");
    handle_error(songMetadata->artist == NULL, "strdup(artist): %s", fullpath);

    char *album = taglib_tag_album(tag);
    songMetadata->album = (album && strlen(album) > 0) ? strdup(album) : strdup("Unknown");
    handle_error(songMetadata->album == NULL, "strdup(album): %s", fullpath);

    char *comment = taglib_tag_comment(tag);
    songMetadata->comment = (comment && strlen(comment) > 0) ? strdup(comment) : strdup("Unknown");
    handle_error(songMetadata->comment == NULL, "strdup(comment): %s", fullpath);

    char *genre = taglib_tag_genre(tag);
    songMetadata->genre = (genre && strlen(genre) > 0) ? strdup(genre) : strdup("Unknown");
    handle_error(songMetadata->genre == NULL, "strdup(genre): %s", fullpath);

    songMetadata->year = taglib_tag_year(tag);
    songMetadata->track = taglib_tag_track(tag);

    taglib_tag_free_strings();
    taglib_file_free(song);
    free(fullpath);
    return 0;
}


void dir_recurse (DIR *parent, int level) {
    struct dirent *ent;
    if (!parent) {
        return;
    }
    while ((ent = readdir(parent)) != NULL) {
        if ((strcmp(ent->d_name, ".") == 0) ||
            (strcmp(ent->d_name, "..") == 0)) {
            continue;
        }

        int parent_fd = dirfd(parent);
        handle_error(parent_fd == -1, "dir fd")
        if (parent_fd < 0) {
            perror("dirfd");
            continue;
        }
        int fd = openat(parent_fd, ent->d_name, O_RDONLY | O_DIRECTORY);
        if (fd != -1) {

            DIR *child = fdopendir(fd);
            if (child) {
                dir_recurse(child, level + 1);
                closedir(child);
            } else {
                perror("fdopendir");
            }
        } else if (errno == ENOTDIR) {
            song_metadata m;
            if (get_name(ent->d_name, get_path_from_fd(parent_fd), &m) != -1) {
                printf("%s\t%s\t%s\t%s\t%s\t%d\t%d\n", m.title, m.artist, m.album, m.comment, m.genre, m.year, m.track);
                free_song_metadata(&m);
            }

        } else {
            perror("openat");
        }
    }
}

int main() {

    const char* dirname = "../Music/";

    DIR *dirP = opendir(dirname);
    handle_error(dirP == NULL, "open dir: %s", dirname)

    dir_recurse(dirP, 0);

    handle_error(closedir(dirP) == -1, "close dir")


    return 0;
}
