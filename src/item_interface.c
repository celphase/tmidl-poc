#include <tmidl.h>

#include "item_interface.h"
#include "mpc_utils.h"

#include <stdio.h>

static mpc_val_t *fold_fields(int n, mpc_val_t **xs)
{
    api_function_t **functions = malloc(sizeof(api_function_t *) * n);

    for (int i = 0; i < n; i++)
    {
        api_function_t *function = malloc(sizeof(api_function_t));
        function->name = xs[i];
        functions[i] = function;
    }

    util_array_t *array = malloc(sizeof(util_array_t));
    array->count = n;
    array->ptr = functions;
    return array;
}

static void free_fields(void *value)
{
    util_array_t *array = value;
    api_function_t **functions = array->ptr;

    for (int i = 0; i < array->count; i++)
    {
        free(functions[i]);
    }

    free(array);
}

static mpc_val_t *fold_interface(int n, mpc_val_t **xs)
{
    api_item_t *item = malloc(sizeof(api_item_t));

    item->type = ITEM_INTERFACE;
    item->name = malloc(strlen(xs[4]) + 1);
    item->doc = NULL;
    strcpy(item->name, xs[4]);

    util_array_t *array = xs[2];
    item->functions = array->ptr;
    item->functions_count = array->count;

    mpcf_all_free(n, xs);
    return item;
}

mpc_parser_t *interface_item()
{
    mpc_parser_t *content_c1 = mpc_oneof(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_*;/. \n\r\t");
    mpc_parser_t *content1 = mpc_many(mpcf_all_free, content_c1);
    mpc_parser_t *content_c2 = mpc_oneof(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_*;/. \n\r\t");
    mpc_parser_t *content2 = mpc_many(mpcf_all_free, content_c2);

    mpc_parser_t *field = mpc_and(
        7, mpcf_trd_free,
        content1, mpc_string("(*"), identifier(), mpc_string(")("), content2, mpc_string(")"),
        mpc_tok(mpc_string(";")),
        free, free, free, free, free, free);
    mpc_parser_t *fields = mpc_many(fold_fields, field);

    return mpc_and(
        6, fold_interface,
        td_struct(), mpc_tok(mpc_char('{')), fields, mpc_tok(mpc_char('}')),
        mpc_tok(identifier()), mpc_char(';'),
        free, free, free_fields, free, free);
}
