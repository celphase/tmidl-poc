#pragma once

#include "mpc.h"

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
