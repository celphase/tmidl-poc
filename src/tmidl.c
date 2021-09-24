#include <tmidl.h>

#include "mpc.h"

mpc_parser_t *any_newline()
{
    return mpc_expect(mpc_or(2, mpc_string("\r\n"), mpc_char('\n')), "newline");
}

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    mpc_parser_t *ident = mpc_new("ident");
    mpc_parser_t *typedef_struct = mpc_new("typedef_struct");
    mpc_parser_t *tmidl = mpc_new("tmidl");

    mpc_parser_t *ident_def = mpc_expect(mpc_ident(), "identifier");
    mpc_define(ident, ident_def);

    mpc_parser_t *pragma_once = mpc_and(
        2, mpcf_strfold,
        mpc_string("#pragma once"), any_newline(),
        free);

    mpc_parser_t *typedef_struct_def = mpc_and(
        2, mpcf_strfold,
        mpc_string("typedef struct"), mpc_strip(ident),
        free);
    mpc_define(typedef_struct, typedef_struct_def);

    // Opaque item
    mpc_parser_t *opaque_item = mpc_and(
        3, mpcf_strfold,
        typedef_struct, mpc_strip(ident), mpc_char(';'),
        free, free);

    // Interface item
    mpc_parser_t *content_c = mpc_oneof(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_()*; \n\r\t");
    mpc_parser_t *content = mpc_many(mpcf_strfold, content_c);
    mpc_parser_t *interface_item = mpc_and(
        6, mpcf_strfold,
        typedef_struct, mpc_strip(mpc_char('{')), content, mpc_strip(mpc_char('}')),
        mpc_strip(ident), mpc_char(';'),
        free, free, free, free, free);

    // Items aggregate
    mpc_parser_t *item = mpc_strip(mpc_or(2, opaque_item, interface_item));
    mpc_parser_t *items = mpc_many(mpcf_strfold, item);

    mpc_parser_t *tmidl_def = mpc_whole(mpc_and(2, mpcf_strfold, pragma_once, items, free), free);
    mpc_define(tmidl, tmidl_def);

    mpc_result_t r;
    bool success = false;
    if (mpc_parse("input", input, tmidl, &r))
    {
        printf("Output: %s\n", r.output);
        free(r.output);
        success = true;
    }
    else
    {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }

    mpc_cleanup(3, ident, typedef_struct, tmidl);

    return success;
}
