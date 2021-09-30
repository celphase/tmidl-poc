#pragma once

#include "../types.h"

typedef enum c_storage_class_t {
    C_STORAGE_CLASS_NONE,
    C_STORAGE_CLASS_TYPEDEF,
} c_storage_class_t;

typedef struct c_type_specifier_struct_t {
    char *name;
} c_type_specifier_struct_t;

typedef struct c_declaration_t {
    const char *doc;
    c_storage_class_t storage_class;
    c_type_specifier_struct_t *type_specifier;
    const char *declarator;
} c_declaration_t;

void free_declaration(c_declaration_t *declaration);

mpc_parser_t *declaration_parser();
