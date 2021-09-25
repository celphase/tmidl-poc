#include <tmidl.h>

#include "doc_comment.h"
#include "item_interface.h"
#include "item.h"
#include "mpc_utils.h"

static mpc_val_t *fold_opaque(int n, mpc_val_t **xs, int x)
{
    api_item_t *item = malloc(sizeof(api_item_t));
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

static mpc_val_t *fold_commented_item(int n, mpc_val_t **xs)
{
    api_item_t *item = xs[1];
    item->doc = xs[0];

    if (item->doc == NULL) {
        item->doc = malloc(1);
        item->doc[0] = '\0';
    }

    return item;
}

mpc_parser_t *any_item()
{
    mpc_parser_t *any_item = mpc_or(2, opaque_item(), interface_item());
    return mpc_and(
        2, fold_commented_item,
        mpc_maybe(doc_comment()), any_item,
        free);
}

void free_item(api_item_t *item) {
    free(item->name);
    free(item->doc);
    free(item);
}
