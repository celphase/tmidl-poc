#include "doc_comment.h"
#include "mpc_utils.h"

mpc_val_t *apply_comment_line(mpc_val_t *x)
{
    char *trimmed = mpcf_strtriml(mpcf_strtrimr(x));
    trimmed = realloc(trimmed, strlen(trimmed) + 2);
    return strcat(trimmed, "\n");
}

static mpc_parser_t *no_newline_whitespace() {
    return mpc_oneof(" \t");
}

mpc_parser_t *parse_doc_comment()
{
    mpc_parser_t *comment_contents = mpc_apply(
        until_eol(),
        apply_comment_line);

    mpc_parser_t *comment_line = mpc_and(
        4, mpcf_trd_free,
        mpc_many(mpcf_all_free, no_newline_whitespace()), mpc_string("//"), comment_contents, any_newline(),
        free, free);

    return mpc_tok(mpc_many1(mpcf_strfold, comment_line));
}
