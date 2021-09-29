#include "c_declaration.h"
#include "mpc_utils.h"
#include "doc_comment.h"

void free_declaration(c_declaration_t *declaration)
{
    free(declaration->name);
    free(declaration->doc);
    free(declaration);
}

static mpc_val_t *fold_declaration(int n, mpc_val_t **xs)
{
    c_declaration_t *declaration = malloc(sizeof(c_declaration_t));

    declaration->storage_class = C_STORAGE_CLASS_NONE;
    declaration->name = xs[3];
    declaration->doc = xs[0];

    if (declaration->doc == NULL)
    {
        declaration->doc = malloc(1);
        declaration->doc[0] = '\0';
    }

    free(xs[1]);
    free(xs[2]);
    free(xs[4]);
    free(xs[5]);
    free(xs[6]);

    return declaration;
}

mpc_parser_t *declaration_parser()
{
    mpc_parser_t *typedef_tok = mpc_tok(mpc_string("typedef"));
    mpc_parser_t *struct_tok = mpc_tok(mpc_string("struct"));

    mpc_parser_t *body_c = mpc_and(2, mpcf_all_free, mpc_not(mpc_char('}'), free), mpc_any(), free);
    mpc_parser_t *body = mpc_and(
        3, mpcf_all_free,
        mpc_tok(mpc_char('{')), mpc_many(mpcf_all_free, body_c), mpc_tok(mpc_char('}')),
        free, free);

    return mpc_and(
        7, fold_declaration,
        mpc_maybe(doc_comment_parser()), mpc_maybe(typedef_tok), struct_tok,
        // Type specifier's name
        mpc_tok(identifier()),
        mpc_maybe(body),
        // Declarator
        mpc_tok(identifier()),
        mpc_tok(mpc_char(';')),
        free, free, free, free, free, free);
}
