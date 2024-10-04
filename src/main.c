#include "dir.h"
#include "song.h"

int main() {

    set_log(0);

    const char* dirname = "../Music/";

    DIR *dirP = opendir(dirname);
    handle_error(dirP == NULL, "open dir: %s", dirname)

    dir_recurse(dirP, 0);

    handle_error(closedir(dirP) == -1, "close dir")


    return 0;
}
