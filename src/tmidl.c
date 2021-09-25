#include <tmidl.h>

#include "mpc.h"

typedef struct context_o {
    const tmidl_callbacks_i *callbacks;
    void *user_context;
} context_o;

static mpc_val_t *fold_nth_free(int n, mpc_val_t **xs, int x) {
    int i;
    for (i = 0; i < n; i++) {
        if (i != x) { free(xs[i]); }
    }
    return xs[x];
}
static mpc_val_t *fold_fth(int n, mpc_val_t **xs) { return fold_nth_free(n, xs, 4); }

static mpc_parser_t *any_newline()
{
    return mpc_expect(mpc_or(2, mpc_string("\r\n"), mpc_char('\n')), "newline");
}

static mpc_parser_t *identifier()
{
    return mpc_expect(mpc_ident(), "identifier");
}

static mpc_parser_t *td_struct()
{
    return mpc_and(
        2, mpcf_snd,
        mpc_tok(mpc_string("typedef struct")), mpc_tok(identifier()),
        free);
}

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

static mpc_val_t *apply_interface(mpc_val_t *value, context_o *context) {
    context->callbacks->on_item_interface(value, context->user_context);
    return NULL;
}

static mpc_parser_t *interface_item(context_o *context)
{
    mpc_parser_t *content_c = mpc_oneof(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_()*; \n\r\t");
    mpc_parser_t *content = mpc_many(mpcf_all_free, content_c);
    mpc_parser_t *item = mpc_and(
        6, fold_fth,
        td_struct(), mpc_strip(mpc_char('{')), content, mpc_strip(mpc_char('}')),
        mpc_strip(identifier()), mpc_char(';'),
        free, free, free, free, free);

    return mpc_apply_to(item, apply_interface, context);
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
