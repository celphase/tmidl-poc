cmake_minimum_required(VERSION 3.8)

project(TMIDL
        VERSION 0.1.0
        DESCRIPTION "The Machinery IDL Parser"
        LANGUAGES C
        )

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    add_compile_options(/wd4996)
endif ()

# Main library

add_library(tmidl STATIC
        headers/tmidl.h
        src/mpc/mpc.c
        src/mpc/mpc.h
        src/parser/c_declaration.h
        src/parser/c_declaration.c
        src/parser/doc_comment.c
        src/parser/doc_comment.h
        src/parser/module.c
        src/parser/module.h
        src/parser/mpc_utils.h
        src/tmidl.c
        src/types.h)
set_property(TARGET tmidl PROPERTY C_STANDARD 11)

target_include_directories(tmidl PUBLIC headers)
target_include_directories(tmidl PRIVATE src/mpc)

# Example executable

add_executable(tmidl-read src/tmidl-read.c)
target_link_libraries(tmidl-read PRIVATE tmidl)

# Include static library in install

install(TARGETS tmidl tmidl-read)
