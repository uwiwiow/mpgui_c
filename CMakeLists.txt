cmake_minimum_required(VERSION 3.29)
project(mpgui C)

set(CMAKE_C_STANDARD 23)

find_package(ZLIB)
find_package(TagLib 2.0.0 REQUIRED)

add_executable(mpgui src/main.c
        src/dir.c
        include/dir.h
        src/song.c
        include/song.h)

target_include_directories(mpgui PRIVATE include)
target_link_libraries(mpgui TagLib::tag_c)