cmake_minimum_required(VERSION 3.17)
project(p4)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)

include_directories(.)

add_executable(p5
        main.c
        bfs.c
        bio.c
        deb.c
        errors.c
        fs.c
        p5test.c
        BFSDISK
        )