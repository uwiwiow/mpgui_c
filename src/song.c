#include "song.h"

static int log = 1;

TagLib_Tag* suppress_taglib_errors(TagLib_Tag* (*taglib_function)(TagLib_File*), TagLib_File* song) {
    int saved_stderr = dup(fileno(stderr));

    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, fileno(stderr));

    TagLib_Tag* result = taglib_function(song);

    dup2(saved_stderr, fileno(stderr));
    close(saved_stderr);
    close(devnull);

    return result;
}

TagLib_Tag* taglib_file_tag_wrapper(TagLib_File* song) {
    return taglib_file_tag(song);
}

/*!
 * Set the log level: 1 enabled (default) - 0 disabled
 */
void set_log(int mode) { log = mode; }

void free_song_metadata(song_metadata *m) {
    if (m->title) free(m->title);
    if (m->artist) free(m->artist);
    if (m->album) free(m->album);
    if (m->comment) free(m->comment);
    if (m->genre) free(m->genre);
}

int get_metadata(char* name, char* dirname, song_metadata* songMetadata) {
    size_t len_dirname = strlen(dirname);
    size_t len_name = strlen(name);
    char *fullpath = (char*)malloc(len_dirname + len_name + 2);
    handle_error(fullpath == NULL, "malloc");

    strcpy(fullpath, dirname);
    strcat(fullpath, name);

    TagLib_File *song = taglib_file_new(fullpath);
    handle_error(song == NULL, "taglib_file_new: %s", fullpath);

    TagLib_Tag *tag;
    if (log) { tag = taglib_file_tag(song); }
    else { tag = suppress_taglib_errors(taglib_file_tag_wrapper, song); }

    if (tag == NULL) {
        if (log) fprintf(stderr, "%s -> %s -> %i -> Error:\n\ttaglib_file_tag: %s\n\n", __FILE_NAME__, __FUNCTION__, __LINE__ - 2,  fullpath);
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