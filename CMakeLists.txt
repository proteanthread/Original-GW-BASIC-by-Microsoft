cmake_minimum_required(VERSION 3.20)
project(GW_BASIC C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

include_directories(
    include
    lib
    ${SDL2_INCLUDE_DIRS}
)

# Backend static libraries (Importable directly into BASIC++)
add_library(gw_math_mbf STATIC lib/gw_math_mbf.c)
add_library(gw_sdl2 STATIC lib/gw_sdl2.c)
add_library(gw_memory STATIC lib/gw_memory.c)
add_library(gw_plugin STATIC lib/gw_plugin.c)
add_library(gw_serial STATIC lib/gw_serial.c)

# Core Interpreter Executable
add_executable(gwbasic WIN32
    src/main.c
    src/interp.c
    src/eval.c
    src/tokenizer.c
    src/variables.c
    src/strings.c
    src/fileio.c
    src/console.c
    src/events.c
)

target_link_libraries(gwbasic
    gw_math_mbf
    gw_sdl2
    gw_memory
    gw_plugin
    gw_serial
    ${SDL2_LIBRARIES}
    m
)
