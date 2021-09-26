cmake_minimum_required(VERSION 3.8)

project(TMIDL
    VERSION 0.1.0
    DESCRIPTION "The Machinery IDL Parser"
    LANGUAGES C
)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

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
    src/items.c
    src/items.h
    src/mpc_utils.h
    src/tmidl.c
)
set_property(TARGET tmidl PROPERTY C_STANDARD 11)

target_include_directories(tmidl PUBLIC headers)
target_include_directories(tmidl PRIVATE vendor/mpc)
target_link_libraries(tmidl PRIVATE vendor_mpc)

# Include static library in install

install(TARGETS tmidl vendor_mpc)
