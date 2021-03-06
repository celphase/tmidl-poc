#include <tmidl.h>

#include "parser/c_declaration.h"
#include "parser/module.h"
#include "parser/mpc_utils.h"

typedef struct tmidl_parser_o
{
    mpc_parser_t *declaration_parser;
    mpc_parser_t *module_parser;
} tmidl_parser_o;

tmidl_parser_o *tmidl_parser_create()
{
    tmidl_parser_o *parser = malloc(sizeof(tmidl_parser_o));

    parser->declaration_parser = parse_declaration();
    parser->module_parser = parse_module(parser->declaration_parser);

    return parser;
}

void tmidl_parser_destroy(tmidl_parser_o *parser)
{
    mpc_cleanup(2, parser->module_parser, parser->declaration_parser);
    free(parser);
}

static void validate_declaration_struct(
    c_declaration_t *declaration,
    c_type_specifier_struct_t *type_specifier,
    const tmidl_callbacks_i *callbacks,
    void *user_context)
{
    if (declaration->storage_class != C_STORAGE_CLASS_TYPEDEF) {
        tmidl_diagnostic_t diagnostic = {
            .level = TMIDL_LEVEL_WARNING,
            .message = "Top level structure declaration must have a typedef storage class.",
            .position_start = declaration->position_start,
            .position_end = declaration->position_end,
        };
        callbacks->on_diagnostic(&diagnostic, user_context);
    }

    if (declaration->declarator != NULL && strcmp(declaration->declarator, type_specifier->name) != 0) {
        uint32_t position = type_specifier->name_position;
        tmidl_diagnostic_t diagnostic = {
            .level = TMIDL_LEVEL_WARNING,
            .message = "The type specifier name must be the same as the declarator.",
            .position_start = position,
            .position_end = position + (uint32_t)strlen(type_specifier->name),
        };
        callbacks->on_diagnostic(&diagnostic, user_context);
    }
}

static void handle_declaration_struct(
    c_declaration_t *c_declaration,
    c_type_specifier_struct_t *type_specifier,
    const tmidl_callbacks_i *callbacks,
    void *user_context)
{
    bool is_interface = type_specifier->declarations != NULL;

    validate_declaration_struct(c_declaration, type_specifier, callbacks, user_context);

    tmidl_declaration_t declaration;
    declaration.type = is_interface ? TMIDL_ITEM_INTERFACE : TMIDL_ITEM_OPAQUE;
    declaration.name = type_specifier->name;
    declaration.doc = c_declaration->doc;
    declaration.functions = NULL;
    declaration.functions_count = 0;

    if (is_interface) {
        declaration.functions_count = type_specifier->declarations->count;
        c_declaration_t **declarations = type_specifier->declarations->ptr;
        declaration.functions = malloc(sizeof(size_t) * declaration.functions_count);

        for (size_t i = 0; i < declaration.functions_count; i++) {
            declaration.functions[i] = malloc(sizeof(tmidl_function_t));
            declaration.functions[i]->name = declarations[i]->declarator;
        }
    }

    callbacks->on_declaration(&declaration, user_context);

    // Clean up
    if (declaration.functions != NULL) {
        for (size_t i = 0; i < declaration.functions_count; i++) {
            free(declaration.functions[i]);
        }
        free(declaration.functions);
    }
}

static void handle_declaration(
    c_declaration_t *declaration,
    const tmidl_callbacks_i *callbacks,
    void *user_context)
{
    switch (declaration->type_specifier->type) {
    case C_TYPE_SPECIFIER_IDENT:
        tmidl_diagnostic_t diagnostic = {
            .level = TMIDL_LEVEL_WARNING,
            .message = "Only structure declarations are allowed at the top level.",
            .position_start = declaration->position_start,
            .position_end = declaration->position_end,
        };
        callbacks->on_diagnostic(&diagnostic, user_context);
        break;
    case C_TYPE_SPECIFIER_STRUCT:
        handle_declaration_struct(declaration, (c_type_specifier_struct_t *)(declaration->type_specifier), callbacks, user_context);
        break;
    }
}

bool tmidl_parser_parse(
    tmidl_parser_o *parser,
    const char *input,
    const tmidl_callbacks_i *callbacks,
    void *user_context)
{
    // Parse the input
    mpc_result_t r;
    bool success = mpc_parse("input", input, parser->module_parser, &r);

    if (!success) {
        char *message = mpc_err_string(r.error);

        tmidl_diagnostic_t diagnostic;
        diagnostic.level = TMIDL_LEVEL_ERROR;
        diagnostic.message = message;
        diagnostic.position_start = r.error->state.pos;
        diagnostic.position_end = r.error->state.pos + 1;
        callbacks->on_diagnostic(&diagnostic, user_context);

        free(message);
        mpc_err_delete(r.error);
        return false;
    }

    // Pass items to caller
    util_array_t *array = r.output;
    c_item_t **items = array->ptr;

    for (int i = 0; i < array->count; i++) {
        c_item_t *item = items[i];

        switch (item->type) {
        case C_ITEM_TYPE_DECLARATION:
            handle_declaration(((c_item_declaration_t *)item)->declaration, callbacks, user_context);
            break;
        case C_ITEM_TYPE_MODULE_DOC:
            callbacks->on_module_doc(((c_item_module_doc_t *)item)->doc, user_context);
            break;
        }
    }

    // Clean up the parsed items
    free_items(array);

    return true;
}
