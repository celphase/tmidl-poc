cmake_minimum_required(VERSION 3.7)

project(LibTMIDL
    VERSION 0.1.0
    DESCRIPTION "The Machinery IDL Parser"
    LANGUAGES C
)

add_library(libtmidl STATIC headers/tmidl.h src/tmidl.c)
target_include_directories(libtmidl PUBLIC headers)

set_property(TARGET libtmidl PROPERTY C_STANDARD 11)
