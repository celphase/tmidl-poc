#include <tmidl.h>

#include "context.h"
#include "interface.h"
#include "mpc_utils.h"

static mpc_val_t *apply_opaque(mpc_val_t *value, context_o *context) {
    context->callbacks->on_item_opaque(value, context->user_context);
    return NULL;
}

static mpc_parser_t *opaque_item(context_o *context)
{
    mpc_parser_t *item = mpc_and(
        3, mpcf_fst_free,
        td_struct(), mpc_strip(identifier()), mpc_char(';'),
        free, free);

    return mpc_apply_to(item, apply_opaque, context);
}

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    context_o context;
    context.callbacks = callbacks;
    context.user_context = user_context;

    mpc_parser_t *tmidl = mpc_new("tmidl");

    mpc_parser_t *pragma_once = mpc_and(
        2, mpcf_all_free,
        mpc_string("#pragma once"), any_newline(),
        free);

    // Items aggregate
    mpc_parser_t *item = mpc_strip(mpc_or(2, opaque_item(&context), interface_item(&context)));
    mpc_parser_t *items = mpc_many(mpcf_all_free, item);

    // Main tmidl definition
    mpc_parser_t *tmidl_def = mpc_whole(mpc_and(2, mpcf_all_free, pragma_once, items, free), free);
    mpc_define(tmidl, tmidl_def);

    // Parse the input
    mpc_result_t r;
    bool success = mpc_parse("input", input, tmidl, &r);

    if (!success)
    {
        // TODO: Callback error
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }

    mpc_cleanup(1, tmidl);

    return success;
}
