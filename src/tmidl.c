#include <tmidl.h>

#include "doc_comment.h"
#include "interface.h"
#include "mpc_utils.h"

static mpc_val_t *fold_opaque(int n, mpc_val_t **xs, int x)
{
    item_o *item = malloc(sizeof(item_o));
    item->type = ITEM_OPAQUE;
    item->name = malloc(strlen(xs[0]) + 1);
    item->doc = NULL;
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

static free_items(items_o *items)
{
    for (int i = 0; i < items->count; i++)
    {
        free(items->items[i].name);
        free(items->items[i].doc);
    }
    free(items->items);
    free(items);
}

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

static mpc_val_t *fold_commented_item(int n, mpc_val_t **xs)
{
    item_o *item = xs[1];
    item->doc = xs[0];

    return item;
}

mpc_parser_t *api_content()
{
    mpc_parser_t *any_item = mpc_or(2, opaque_item(), interface_item());
    mpc_parser_t *commented_item = mpc_and(
        2, fold_commented_item,
        mpc_maybe(doc_comment()), any_item,
        free);

    return mpc_many(fold_items, mpc_strip(commented_item));
}

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    mpc_parser_t *tmidl = mpc_new("tmidl");

    mpc_parser_t *pragma_once = mpc_and(
        2, mpcf_all_free,
        mpc_string("#pragma once"), any_newline(),
        free);

    // Main tmidl definition
    mpc_parser_t *tmidl_def = mpc_whole(
        mpc_and(2, mpcf_snd_free, pragma_once, api_content(), free),
        free_items);
    mpc_define(tmidl, tmidl_def);

    // Parse the input
    mpc_result_t r;
    bool success = mpc_parse("input", input, tmidl, &r);
    mpc_cleanup(1, tmidl);

    if (!success)
    {
        char *message = mpc_err_string(r.error);
        callbacks->on_error(message, r.error->state.pos, user_context);

        free(message);
        mpc_err_delete(r.error);
        return false;
    }

    // Pass items to caller
    items_o *items = r.output;

    for (int i = 0; i < items->count; i++)
    {
        callbacks->on_item(&items->items[i], user_context);
    }

    // Clean up the parsed items
    free_items(items);

    return true;
}
