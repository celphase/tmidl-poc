#pragma once

typedef struct tmidl_callbacks_i tmidl_callbacks_i;
typedef struct api_item_t api_item_t;

typedef void mpc_val_t;
typedef struct mpc_parser_t mpc_parser_t;

typedef struct c_declaration_t c_declaration_t;

typedef struct util_array_t {
    void *ptr;
    int count;
} util_array_t;
