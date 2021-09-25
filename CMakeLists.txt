cmake_minimum_required(VERSION 3.8)

project(TMIDL
    VERSION 0.1.0
    DESCRIPTION "The Machinery IDL Parser"
    LANGUAGES C
)

# MPC parsing library dependency

add_library(vendor_mpc STATIC
    vendor/mpc/mpc.c
    vendor/mpc/mpc.h
)
set_property(TARGET vendor_mpc PROPERTY C_STANDARD 11)

# Main library

add_library(tmidl STATIC
    headers/tmidl.h
    src/doc_comment.c
    src/doc_comment.h
    src/interface.c
    src/interface.h
    src/mpc_utils.h
    src/tmidl.c
)
set_property(TARGET tmidl PROPERTY C_STANDARD 11)

target_include_directories(tmidl PUBLIC headers)
target_include_directories(tmidl PRIVATE vendor/mpc)
target_link_libraries(tmidl PRIVATE vendor_mpc)

# Include static library in install

install(TARGETS tmidl vendor_mpc)
