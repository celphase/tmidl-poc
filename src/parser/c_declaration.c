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
    declaration->name = malloc(1);
    declaration->doc = malloc(1);

    return declaration;
}

mpc_parser_t *declaration_parser()
{
    mpc_parser_t *typedef_tok = mpc_tok(mpc_string("typedef"));
    mpc_parser_t *struct_tok = mpc_tok(mpc_string("struct"));

    return mpc_and(
        3, fold_declaration,
        mpc_maybe(doc_comment_parser()), mpc_maybe(typedef_tok), struct_tok,
        free, free);
}
