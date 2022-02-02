cmake_minimum_required(VERSION 3.16)
project(SASAT C)

set(CMAKE_C_STANDARD 11)

add_executable(SASAT
        main.c mem.h mem.c io.h io.c core.h core.c)

target_link_libraries(SASAT m)