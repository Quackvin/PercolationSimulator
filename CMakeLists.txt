cmake_minimum_required(VERSION 3.8)
project(git)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        flexiParaPerc.c
        paraPerc.c
        perc.c)

add_executable(git ${SOURCE_FILES})