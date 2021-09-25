#pragma once

typedef struct tmidl_callbacks_i tmidl_callbacks_i;

typedef void mpc_val_t;
typedef struct mpc_parser_t mpc_parser_t;

typedef struct item_o
{
    enum Type
    {
        ITEM_OPAQUE,
        ITEM_INTERFACE
    } type;
    char *name;
    char *comment;
} item_o;
