#include <tmidl.h>

#include "mpc.h"

mpc_parser_t *any_newline()
{
    return mpc_expect(mpc_or(2, mpc_string("\r\n"), mpc_char('\n')), "newline");
}

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    mpc_parser_t *pragma_once = mpc_and(
        2, mpcf_strfold,
        mpc_string("#pragma once"), any_newline(),
        free);

    mpc_parser_t *item = mpc_strip(mpc_string("typedef struct"));
    mpc_parser_t *items = mpc_many(mpcf_strfold, item);

    mpc_parser_t *tmidl = mpc_new("tmidl");
    mpc_define(tmidl, mpc_whole(mpc_and(2, mpcf_strfold, pragma_once, items, free), free));

    mpc_result_t r;
    if (mpc_parse("input", input, tmidl, &r))
    {
        mpc_ast_print(r.output);
        mpc_ast_delete(r.output);
    }
    else
    {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }

    mpc_cleanup(1, tmidl);

    return false;
}
