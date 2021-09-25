#include <tmidl.h>

#include "interface.h"
#include "mpc_utils.h"

static mpc_val_t *fold_opaque(int n, mpc_val_t **xs, int x)
{
    item_o *item = malloc(sizeof(item_o));
    item->type = ITEM_OPAQUE;
    item->name = malloc(strlen(xs[0]) + 1);
    strcpy(item->name, xs[0]);

    mpcf_all_free(n, xs, x);
    return item;
}

static mpc_parser_t *opaque_item()
{
    return mpc_and(
        3, fold_opaque,
        td_struct(), mpc_strip(identifier()), mpc_char(';'),
        free, free);
}

typedef struct items_o
{
    int count;
    item_o *items;
} items_o;

static mpc_val_t *fold_items(int n, mpc_val_t **xs)
{
    items_o *items = malloc(sizeof(items_o));
    items->count = 0;

    // Count the items
    for (int i = 0; i < n; i++)
    {
        if (xs[i] != NULL)
        {
            items->count += 1;
        }
    }

    // Allocate the array
    items->items = malloc(sizeof(item_o) * items->count);
    int items_i = 0;
    for (int i = 0; i < n; i++)
    {
        if (xs[i] != NULL)
        {
            items->items[items_i] = *(item_o *)xs[i];
            free(xs[i]);
            items_i += 1;
        }
    }

    return items;
}

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    mpc_parser_t *tmidl = mpc_new("tmidl");

    mpc_parser_t *pragma_once = mpc_and(
        2, mpcf_all_free,
        mpc_string("#pragma once"), any_newline(),
        free);

    // Items aggregate
    mpc_parser_t *item = mpc_strip(mpc_or(2, opaque_item(), interface_item()));
    mpc_parser_t *items = mpc_many(fold_items, item);

    // Main tmidl definition
    mpc_parser_t *tmidl_def = mpc_whole(mpc_and(2, mpcf_snd_free, pragma_once, items, free), free);
    mpc_define(tmidl, tmidl_def);

    // Parse the input
    mpc_result_t r;
    bool success = mpc_parse("input", input, tmidl, &r);
    mpc_cleanup(1, tmidl);

    if (!success)
    {
        char *message = mpc_err_string(r.error);
        callbacks->on_error(message, user_context);

        free(message);
        mpc_err_delete(r.error);
        return false;
    }

    // Pass items to caller
    items_o *values = r.output;

    for (int i = 0; i < values->count; i++)
    {
        item_o *item = &values->items[i];

        switch (item->type)
        {
        case ITEM_OPAQUE:
            callbacks->on_item_opaque(values->items[i].name, user_context);
            break;
        case ITEM_INTERFACE:
            callbacks->on_item_interface(values->items[i].name, user_context);
            break;
        }
    }

    free(values->items);
    free(values);

    return true;
}
