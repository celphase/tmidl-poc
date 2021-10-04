#include "c_declaration.h"
#include "mpc_utils.h"
#include "doc_comment.h"

static mpc_val_t *fold_field(int n, mpc_val_t **xs)
{
    free(xs[0]);
    free(xs[1]);
    free(xs[3]);
    free(xs[4]);

    return xs[2];
}

// TODO: Recursive declarations
static mpc_parser_t *field_parser()
{
    mpc_parser_t *before = mpc_and(2, mpcf_all_free, mpc_not(mpc_char('('), free), mpc_any(), free);
    mpc_parser_t *after = mpc_and(2, mpcf_all_free, mpc_not(mpc_char(';'), free), mpc_any(), free);

    mpc_parser_t *name = mpc_and(
        2, mpcf_snd_free,
        mpc_char('*'), identifier());

    return mpc_and(
        5, fold_field,
        doc_comment_parser(),
        mpc_many(mpcf_all_free, before),
        mpc_tok_parens(name, free),
        mpc_many(mpcf_all_free, after),
        mpc_tok(mpc_char(';')),
        free, free, free);
}

static mpc_val_t *fold_body(int n, mpc_val_t **xs)
{
    char **declarations = malloc(sizeof(size_t) * n);

    for (int i = 0; i < n; i++)
    {
        declarations[i] = xs[i];
    }

    util_array_t *array = malloc(sizeof(util_array_t));
    array->count = n;
    array->ptr = declarations;
    return array;
}

static mpc_parser_t *body_parser()
{
    mpc_parser_t *fields = mpc_many(fold_body, field_parser());
    return mpc_tok_brackets(fields, free);
}

static mpc_val_t *fold_type_specifier_struct(int n, mpc_val_t **xs)
{
    mpc_state_t *state = xs[1];

    c_type_specifier_struct_t *type_specifier = malloc(sizeof(c_type_specifier_struct_t));
    type_specifier->name = xs[2];
    type_specifier->name_position = state->pos;
    type_specifier->declarations = xs[3];

    free(xs[0]);
    free(xs[1]);

    return type_specifier;
}

static mpc_parser_t *type_specifier_struct_parser()
{
    mpc_parser_t *struct_tok = mpc_tok(mpc_string("struct"));

    return mpc_and(
        4, fold_type_specifier_struct,
        struct_tok,
        // Name
        mpc_state(), mpc_tok(identifier()),
        mpc_maybe(body_parser()),
        free, free, free);
}

static void free_type_specifier(c_type_specifier_struct_t *type_specifier)
{
    free(type_specifier->name);

    if (type_specifier->declarations != NULL)
    {
        char **declarations = type_specifier->declarations->ptr;
        for (int i = 0; i < type_specifier->declarations->count; i++)
        {
            free(declarations[i]);
        }
        free(type_specifier->declarations);
    }

    free(type_specifier);
}

void free_declaration(c_declaration_t *declaration)
{
    free(declaration->doc);
    free_type_specifier(declaration->type_specifier);
    free(declaration->declarator);

    free(declaration);
}

static mpc_val_t *fold_declaration(int n, mpc_val_t **xs)
{
    mpc_state_t *state = xs[3];
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
