cmake_minimum_required(VERSION 4.2)
project(system_4711 C)

set(CMAKE_C_STANDARD 11)

add_executable(system_4711 main_core.c
        database_4711.h
        database_4711.c
        language.h
        language.c
        core_ui.c
        core_ui.h
        assets.c
        assets.h
        core_buffer.c
        core_buffer.h
        core_char.c
        core_char.h)
