#include "c_declaration.h"
#include "doc_comment.h"
#include "mpc_utils.h"

static void free_array_declarations(void *value)
{
    util_array_t *array = value;
    c_declaration_t **declarations = array->ptr;

    for (int i = 0; i < array->count; i++) {
        free_declaration(declarations[i]);
    }

    free(array->ptr);
    free(array);
}

static mpc_val_t *fold_body(int n, mpc_val_t **xs)
{
    char **declarations = malloc(sizeof(size_t) * n);

    for (int i = 0; i < n; i++) {
        declarations[i] = xs[i];
    }

    util_array_t *array = malloc(sizeof(util_array_t));
    array->count = n;
    array->ptr = declarations;
    return array;
}

static mpc_parser_t *parse_body(mpc_parser_t *declaration)
{
    mpc_parser_t *fields = mpc_many(fold_body, declaration);
    return mpc_tok_brackets(fields, free_array_declarations);
}

static mpc_val_t *fold_type_specifier_struct(int n, mpc_val_t **xs)
{
    mpc_state_t *state = xs[1];

    c_type_specifier_struct_t *type_specifier = malloc(sizeof(c_type_specifier_struct_t));
    type_specifier->base.type = C_TYPE_SPECIFIER_STRUCT;
    type_specifier->name = xs[2];
    type_specifier->name_position = state->pos;
    type_specifier->declarations = xs[3];

    free(xs[0]);
    free(state);

    return type_specifier;
}

static mpc_parser_t *parse_type_specifier_struct(mpc_parser_t *declaration)
{
    mpc_parser_t *struct_tok = mpc_sym("struct");

    return mpc_and(
        4, fold_type_specifier_struct,
        struct_tok,
        // Name
        mpc_state(), mpc_tok(identifier()),
        mpc_maybe(parse_body(declaration)),
        free, free, free);
}

static mpc_val_t *apply_type_specifier_ident(mpc_val_t *value)
{
    c_type_specifier_ident_t *type_specifier = malloc(sizeof(c_type_specifier_ident_t));
    type_specifier->base.type = C_TYPE_SPECIFIER_IDENT;
    type_specifier->name = value;

    return type_specifier;
}

static mpc_parser_t *parse_type_specifier_ident()
{
    return mpc_apply(mpc_tok(identifier()), apply_type_specifier_ident);
}

static mpc_parser_t *parse_type_specifier(mpc_parser_t *declaration)
{
    return mpc_or(2, parse_type_specifier_struct(declaration), parse_type_specifier_ident());
}

static mpc_parser_t *parse_declarator()
{
    // We right now just ignore the pointer symbols and parens
    mpc_parser_t *ptr_sym1 = mpc_tok(mpc_char('*'));
    mpc_parser_t *ptr_sym2 = mpc_tok(mpc_char('*'));

    mpc_parser_t *name = mpc_or(2,
        identifier(),
        mpc_tok_parens(mpc_and(2, mpcf_snd_free, ptr_sym2, identifier(), free), free));

    mpc_parser_t *parameters_c = mpc_and(2, mpcf_all_free, mpc_not(mpc_char(')'), free), mpc_any(), free);

    return mpc_and(
        3, mpcf_snd_free,
        mpc_maybe(ptr_sym1),
        name,
        mpc_maybe(mpc_parens(mpc_many(mpcf_all_free, parameters_c), free)),
        free, free);
}

static void free_type_specifier_ident(c_type_specifier_ident_t *type_specifier)
{
    free(type_specifier->name);
}

static void free_type_specifier_struct(c_type_specifier_struct_t *type_specifier)
{
    free(type_specifier->name);

    if (type_specifier->declarations != NULL) {
        free_array_declarations(type_specifier->declarations);
    }
}

static void free_type_specifier(c_type_specifier_t *type_specifier)
{
    switch (type_specifier->type) {
    case C_TYPE_SPECIFIER_IDENT:
        free_type_specifier_ident((c_type_specifier_ident_t *)type_specifier);
        break;
    case C_TYPE_SPECIFIER_STRUCT:
        free_type_specifier_struct((c_type_specifier_struct_t *)type_specifier);
        break;
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
    mpc_state_t *start_state = xs[0];
    mpc_state_t *end_state = xs[6];

    c_declaration_t *declaration = malloc(sizeof(c_declaration_t));

    declaration->doc = xs[1];
    declaration->storage_class = C_STORAGE_CLASS_NONE;
    declaration->type_specifier = xs[3];
    declaration->declarator = xs[4];
    declaration->position_start = start_state->pos;
    declaration->position_end = end_state->pos;

    if (declaration->doc == NULL) {
        char *doc = malloc(1);
        doc[0] = '\0';
        declaration->doc = doc;
    }

    free(start_state);
    free(xs[2]);
    free(xs[5]);
    free(end_state);

    return declaration;
}

mpc_parser_t *parse_declaration()
{
    mpc_parser_t *declaration = mpc_new("declaration");
    mpc_parser_t *typedef_tok = mpc_sym("typedef");

    // While C allows specifiers in any order, TMIDL enforces an order.
    mpc_parser_t *declaration_def = mpc_and(
        7, fold_declaration,
        mpc_state(),
        mpc_maybe(parse_doc_comment()),
        // Storage class specifier
        mpc_maybe(typedef_tok),
        parse_type_specifier(declaration),
        // Declarator
        parse_declarator(),
        mpc_char(';'),
        mpc_state(),
        free, free, free, free, free, free);
    mpc_define(declaration, mpc_tok(declaration_def));

    return declaration;
}
