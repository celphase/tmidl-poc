#pragma once

#include "../types.h"

typedef enum c_item_type_t {
    C_ITEM_TYPE_DECLARATION,
    C_ITEM_TYPE_MODULE_DOC,
} c_item_type_t;

// Items can right now only be declarations, but this will be extended later for preprocessor
// directives like `if`.
typedef struct c_item_t
{
    c_item_type_t type;
} c_item_t;

typedef struct c_item_declaration_t
{
    c_item_t base;
    c_declaration_t *declaration;
} c_item_declaration_t;

typedef struct c_item_module_doc_t
{
    c_item_t base;
    char *doc;
} c_item_module_doc_t;

void free_items(mpc_val_t *value);

mpc_parser_t *parse_api_file(mpc_parser_t *declaration);
