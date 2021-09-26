#include <tmidl.h>

#include "types.h"
#include "items.h"
#include "mpc_utils.h"

static mpc_val_t *fold_items(int n, mpc_val_t **xs)
{
    util_array_t *array = malloc(sizeof(util_array_t));
    array->count = 0;

    // Count the items
    for (int i = 0; i < n; i++)
    {
        if (xs[i] != NULL)
        {
            array->count += 1;
        }
    }

    // Allocate the array
    api_item_t **items = malloc(sizeof(api_item_t) * array->count);
    array->ptr = items;
    int items_i = 0;
    for (int i = 0; i < n; i++)
    {
        if (xs[i] != NULL)
        {
            items[items_i] = xs[i];
            items_i += 1;
        }
    }

    return array;
}

static void free_items(util_array_t *array)
{
    api_item_t **items = array->ptr;
    for (int i = 0; i < array->count; i++)
    {
        free_item(items[i]);
    }
    free(array);
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
    util_array_t *array = r.output;
    api_item_t **items = array->ptr;

    for (int i = 0; i < array->count; i++)
    {
        callbacks->on_item(items[i], user_context);
    }

    // Clean up the parsed items
    free_items(array);

    return true;
}
