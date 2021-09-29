#pragma once

#include "../types.h"

typedef enum c_item_type_t {
    CITEM_DEFINITION,
} c_item_type_t;

// Items can right now only be declarations, but this will be exctended later for preprocessor
// directives like `if`.
typedef struct c_item_t {
    c_item_type_t type;
} c_item_t;

typedef struct c_item_declaration_t {
    c_declaration_t *declaration;
} c_item_declaration_t;

void free_items(util_array_t *array);

mpc_parser_t *api_file();
