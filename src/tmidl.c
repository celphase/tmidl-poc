#include <tmidl.h>

#include "types.h"
#include "parser/api_file.h"
#include "parser/mpc_utils.h"
#include "parser/c_declaration.h"

static void validate_declaration(
    c_declaration_t *declaration,
    const tmidl_callbacks_i *callbacks,
    void *user_context)
{
    bool success = true;

    if (strcmp(declaration->declarator, declaration->type_specifier->name) != 0)
    {
        tmidl_diagnostic_t diagnostic;
        diagnostic.level = TMIDL_LEVEL_WARNING;
        diagnostic.message = "The type specifier name and declarator must be the same.";
        diagnostic.position_start = declaration->declarator_pos;
        diagnostic.position_end = declaration->declarator_pos + strlen(declaration->declarator);
        callbacks->on_diagnostic(&diagnostic, user_context);

        success = false;
    }

    return success;
}

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    // Parse the input
    mpc_result_t r;
    bool success = mpc_parse("input", input, api_file_parser(), &r);

    if (!success)
    {
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

    for (int i = 0; i < array->count; i++)
    {
        c_item_t *item = items[i];

        if (item->type == C_ITEM_TYPE_DECLARATION)
        {
            c_item_declaration_t *item_declaration = item;
            c_declaration_t *c_declaration = item_declaration->declaration;

            validate_declaration(c_declaration, callbacks, user_context);

            tmidl_declaration_t declaration;
            declaration.type = ITEM_OPAQUE;
            declaration.name = c_declaration->declarator;
            declaration.doc = c_declaration->doc;
            declaration.functions = NULL;
            declaration.functions_count = 0;

            callbacks->on_declaration(&declaration, user_context);
        }
    }

    // Clean up the parsed items
    free_items(array);

    return true;
}
