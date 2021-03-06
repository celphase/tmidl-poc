#include <tmidl.h>

#include "c_declaration.h"
#include "doc_comment.h"
#include "module.h"
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

static mpc_parser_t *declaration_item(mpc_parser_t *declaration)
{
    return mpc_apply(declaration, apply_declaration);
}

static mpc_val_t *apply_module_doc_item(mpc_val_t *value)
{
    c_item_module_doc_t *item = malloc(sizeof(c_item_module_doc_t));
    item->base.type = C_ITEM_TYPE_MODULE_DOC;
    item->doc = value;

    return item;
}

static mpc_parser_t *module_doc_item()
{
    return mpc_apply(parse_doc_comment(), apply_module_doc_item);
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

void free_items(mpc_val_t *value)
{
    util_array_t *array = value;

    c_item_t **items = array->ptr;
    for (int i = 0; i < array->count; i++) {
        c_item_t *item = items[i];

        switch (item->type) {
        case C_ITEM_TYPE_DECLARATION:
            free_declaration(((c_item_declaration_t *)item)->declaration);
            break;
        case C_ITEM_TYPE_MODULE_DOC:
            free(((c_item_module_doc_t *)item)->doc);
            break;
        }

        free(item);
    }

    free(array->ptr);
    free(array);
}

mpc_parser_t *parse_module(mpc_parser_t *declaration)
{
    mpc_parser_t *module = mpc_new("module");

    mpc_parser_t *pragma_once = mpc_and(
        2, mpcf_all_free,
        mpc_string("#pragma once"), any_newline(),
        free);

    mpc_parser_t *c_item = mpc_or(
        3,
        include_item(),
        declaration_item(declaration),
        // Intentionally last, as it will otherwise swallow declaration docs
        module_doc_item());
    mpc_parser_t *c_items = mpc_many(fold_items, mpc_strip(c_item));

    mpc_parser_t *content = mpc_and(
        2, mpcf_snd_free,
        pragma_once, c_items,
        free);

    mpc_define(module, mpc_whole(content, free_items));

    return module;
}
