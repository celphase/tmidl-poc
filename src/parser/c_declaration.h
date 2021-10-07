#pragma once

#include "../types.h"

typedef enum c_storage_class_t {
    C_STORAGE_CLASS_NONE,
    C_STORAGE_CLASS_TYPEDEF,
} c_storage_class_t;

typedef struct c_type_specifier_struct_t
{
    char *name;
    long name_position;
    util_array_t *declarations;
} c_type_specifier_struct_t;

typedef struct c_declaration_t
{
    char *doc;
    c_storage_class_t storage_class;
    c_type_specifier_struct_t *type_specifier;
    char *declarator;
} c_declaration_t;

void free_declaration(c_declaration_t *declaration);

mpc_parser_t *parse_declaration();
