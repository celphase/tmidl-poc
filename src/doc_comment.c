#include "doc_comment.h"
#include "mpc_utils.h"

mpc_val_t *apply_comment_line(mpc_val_t *x)
{
    char *trimmed = mpcf_strtriml(mpcf_strtrimr(x));
    return strcat(trimmed, "\n");
}

mpc_parser_t *doc_comment()
{
    mpc_parser_t *comment_char = mpc_and(
        2, mpcf_snd_free,
        mpc_not(any_newline(), free),
        mpc_any(),
        free);
    mpc_parser_t *comment_contents = mpc_apply(
        mpc_many(mpcf_strfold, comment_char),
        apply_comment_line);

    mpc_parser_t *comment_line = mpc_and(
        3, mpcf_snd_free,
        mpc_string("//"), comment_contents, any_newline(),
        free, free);

    return mpc_many1(mpcf_strfold, comment_line);
}
