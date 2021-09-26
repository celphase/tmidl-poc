#pragma once

#include <mpc.h>

static mpc_parser_t *any_newline()
{
    return mpc_expect(mpc_or(2, mpc_string("\r\n"), mpc_char('\n')), "newline");
}

static mpc_parser_t *until_eol()
{
    mpc_parser_t *not_newline = mpc_and(
        2, mpcf_snd,
        mpc_not(any_newline(), free), mpc_any(),
        free);
    return mpc_many(mpcf_strfold, not_newline);
}

static mpc_parser_t *identifier()
{
    return mpc_expect(mpc_ident(), "identifier");
}

static mpc_parser_t *tydef_struct_tok()
{
    return mpc_and(
        3, mpcf_trd,
        mpc_tok(mpc_string("typedef")), mpc_tok(mpc_string("struct")), mpc_tok(identifier()),
        free);
}
