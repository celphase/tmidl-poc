#include <tmidl.h>

#include "item_interface.h"
#include "mpc_utils.h"

static mpc_val_t *fold_interface(int n, mpc_val_t **xs)
{
    item_t *item = malloc(sizeof(item_t));
    item->type = ITEM_INTERFACE;
    item->name = malloc(strlen(xs[4]) + 1);
    item->doc = NULL;
    strcpy(item->name, xs[4]);

    mpcf_all_free(n, xs);
    return item;
}

mpc_parser_t *interface_item()
{
    mpc_parser_t *content_c = mpc_oneof(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_()*;/. \n\r\t");
    mpc_parser_t *content = mpc_many(mpcf_all_free, content_c);

    return mpc_and(
        6, fold_interface,
        td_struct(), mpc_strip(mpc_char('{')), content, mpc_strip(mpc_char('}')),
        mpc_strip(identifier()), mpc_char(';'),
        free, free, free, free, free);
}