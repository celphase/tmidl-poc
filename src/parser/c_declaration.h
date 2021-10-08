#pragma once

#include <stdint.h>
#include "../types.h"

typedef enum c_storage_class_t {
    C_STORAGE_CLASS_NONE,
    C_STORAGE_CLASS_TYPEDEF,
} c_storage_class_t;

typedef enum c_type_specifier_type_t {
    C_TYPE_SPECIFIER_IDENT,
    C_TYPE_SPECIFIER_STRUCT
} c_type_specifier_type_t;

typedef struct c_type_specifier_t {
    c_type_specifier_type_t type;
} c_type_specifier_t;

typedef struct c_type_specifier_struct_t
{
    c_type_specifier_t base;
    char *name;
    uint32_t name_position;
    // Contains c_declaration_t
    util_array_t *declarations;
} c_type_specifier_struct_t;

typedef struct c_type_specifier_ident_t
{
    c_type_specifier_t base;
    char *name;
} c_type_specifier_ident_t;

typedef struct c_declaration_t
{
    char *doc;
    c_storage_class_t storage_class;
    c_type_specifier_t *type_specifier;
    char *declarator;
    uint32_t position_start;
    uint32_t position_end;
} c_declaration_t;

void free_declaration(c_declaration_t *declaration);

mpc_parser_t *parse_declaration();
