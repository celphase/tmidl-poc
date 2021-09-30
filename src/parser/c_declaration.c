#include "c_declaration.h"
#include "mpc_utils.h"
#include "doc_comment.h"

static mpc_val_t *fold_type_specifier_struct(int n, mpc_val_t **xs)
{
    c_type_specifier_struct_t *type_specifier = malloc(sizeof(c_type_specifier_struct_t));
    type_specifier->name = xs[1];

    free(xs[0]);
    free(xs[2]);

    return type_specifier;
}

static mpc_parser_t *type_specifier_struct_parser()
{
    mpc_parser_t *struct_tok = mpc_tok(mpc_string("struct"));

    mpc_parser_t *body_c = mpc_and(2, mpcf_all_free, mpc_not(mpc_char('}'), free), mpc_any(), free);
    mpc_parser_t *body = mpc_and(
        3, mpcf_all_free,
        mpc_tok(mpc_char('{')), mpc_many(mpcf_all_free, body_c), mpc_tok(mpc_char('}')),
        free, free);

    return mpc_and(
        3, fold_type_specifier_struct,
        struct_tok, mpc_tok(identifier()), mpc_maybe(body),
        free, free);
}

void free_declaration(c_declaration_t *declaration)
{
    free(declaration->doc);
    free(declaration->declarator);
    free(declaration->type_specifier->name);
    free(declaration->type_specifier);
    free(declaration);
}

static mpc_val_t *fold_declaration(int n, mpc_val_t **xs)
{
    c_declaration_t *declaration = malloc(sizeof(c_declaration_t));

    declaration->doc = xs[0];
    declaration->storage_class = C_STORAGE_CLASS_NONE;
    declaration->type_specifier = xs[2];
    declaration->declarator = xs[3];

    if (declaration->doc == NULL)
    {
        char *doc = malloc(1);
        doc[0] = '\0';
        declaration->doc = doc;
    }

    free(xs[1]);
    free(xs[4]);

    return declaration;
}

mpc_parser_t *declaration_parser()
{
    mpc_parser_t *typedef_tok = mpc_tok(mpc_string("typedef"));

    // While C allows specifiers in any order, TMIDL enforces an order.
    return mpc_and(
        5, fold_declaration,
        mpc_maybe(doc_comment_parser()),
        // Storage class specifier
        mpc_maybe(typedef_tok),
        type_specifier_struct_parser(),
        // Declarator
        mpc_tok(identifier()),
        mpc_tok(mpc_char(';')),
        free, free, free, free);
}

struct Foo
{
    int a;
} typedef const foo;
