#include <tmidl.h>

#include "types.h"
#include "item.h"
#include "mpc_utils.h"

typedef struct items_t
{
    int count;
    item_t *items;
} items_t;

static free_items(items_t *items)
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
    items_t *items = malloc(sizeof(items_t));
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
    items->items = malloc(sizeof(item_t) * items->count);
    int items_i = 0;
    for (int i = 0; i < n; i++)
    {
        if (xs[i] != NULL)
        {
            items->items[items_i] = *(item_t *)xs[i];
            free(xs[i]);
            items_i += 1;
        }
    }

    return items;
}

mpc_parser_t *api_content()
{
    return mpc_many(fold_items, mpc_strip(any_item()));
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
    items_t *items = r.output;

    for (int i = 0; i < items->count; i++)
    {
        callbacks->on_item(&items->items[i], user_context);
    }

    // Clean up the parsed items
    free_items(items);

    return true;
}
