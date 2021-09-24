cmake_minimum_required(VERSION 3.8)

project(TMIDL
    VERSION 0.1.0
    DESCRIPTION "The Machinery IDL Parser"
    LANGUAGES C
)

add_library(tmidl STATIC headers/tmidl.h src/tmidl.c)
target_include_directories(tmidl PUBLIC headers)
set_property(TARGET tmidl PROPERTY C_STANDARD 11)

install(TARGETS tmidl)
