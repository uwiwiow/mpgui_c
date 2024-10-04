#ifndef MPGUI_SONG_H
#define MPGUI_SONG_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <taglib/tag_c.h>
#include <string.h>
#include <errno.h>

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

void set_log(int mode);

void free_song_metadata(song_metadata *m);

int get_metadata(char* name, char* dirname, song_metadata* songMetadata);

#endif //MPGUI_SONG_H
