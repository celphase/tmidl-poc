#include <tmidl.h>

#include "api_file.h"
#include "c_declaration.h"
#include "doc_comment.h"
#include "mpc_utils.h"

static mpc_parser_t *include_item()
{
    return mpc_and(
        3, mpcf_all_free,
        mpc_char('#'), until_eol(), any_newline(),
        free, free);
}

mpc_val_t *apply_declaration(mpc_val_t *value)
{
    c_item_declaration_t *item = malloc(sizeof(c_item_declaration_t));
    item->base.type = C_ITEM_TYPE_DECLARATION;
    item->declaration = value;

    return item;
}

static mpc_parser_t *declaration_item()
{
    return mpc_apply(declaration_parser(), apply_declaration);
}

static mpc_val_t *fold_items(int n, mpc_val_t **xs)
{
    util_array_t *array = malloc(sizeof(util_array_t));
    array->count = 0;

    // Count the items
    for (int i = 0; i < n; i++) {
        if (xs[i] != NULL) {
            array->count += 1;
        }
    }

    // Allocate the array
    c_item_t **items = malloc(sizeof(c_item_t *) * array->count);
    array->ptr = items;
    int items_i = 0;
    for (int i = 0; i < n; i++) {
        if (xs[i] != NULL) {
            items[items_i] = xs[i];
            items_i += 1;
        }
    }

    return array;
}

void free_items(util_array_t *array)
{
    c_item_t **items = array->ptr;
    for (int i = 0; i < array->count; i++) {
        c_item_t *item = items[i];

        if (item->type == C_ITEM_TYPE_DECLARATION) {
            free_declaration(((c_item_declaration_t *)item)->declaration);
        }

        free(item);
    }
    free(array);
}

mpc_parser_t *api_file_parser()
{
    mpc_parser_t *pragma_once = mpc_and(
        2, mpcf_all_free,
        mpc_string("#pragma once"), any_newline(),
        free);

    mpc_parser_t *c_item = mpc_or(2, include_item(), declaration_item());
    mpc_parser_t *c_items = mpc_many(fold_items, mpc_strip(c_item));

    mpc_parser_t *content = mpc_and(
        2, mpcf_snd_free,
        pragma_once, c_items,
        free);

    return mpc_whole(content, free_items);
}
