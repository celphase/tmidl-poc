#include <tmidl.h>

#include "context.h"
#include "interface.h"
#include "mpc_utils.h"

static mpc_val_t *fold_nth_free(int n, mpc_val_t **xs, int x) {
    int i;
    for (i = 0; i < n; i++) {
        if (i != x) { free(xs[i]); }
    }
    return xs[x];
}

static mpc_val_t *fold_fourth(int n, mpc_val_t **xs) { return fold_nth_free(n, xs, 4); }

mpc_val_t *apply_interface(mpc_val_t *value, context_o *context) {
    context->callbacks->on_item_interface(value, context->user_context);
    return NULL;
}

mpc_parser_t *interface_item(context_o *context)
{
    mpc_parser_t *content_c = mpc_oneof(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_()*; \n\r\t");
    mpc_parser_t *content = mpc_many(mpcf_all_free, content_c);
    mpc_parser_t *item = mpc_and(
        6, fold_fourth,
        td_struct(), mpc_strip(mpc_char('{')), content, mpc_strip(mpc_char('}')),
        mpc_strip(identifier()), mpc_char(';'),
        free, free, free, free, free);

    return mpc_apply_to(item, apply_interface, context);
}
