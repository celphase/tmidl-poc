#include <tmidl.h>

#include "types.h"
#include "parser/api_file.h"
#include "parser/mpc_utils.h"
#include "parser/c_declaration.h"

static bool validate_declaration(
    c_declaration_t *declaration,
    const tmidl_callbacks_i *callbacks,
    void *user_context)
{
    bool success = true;

    if (strcmp(declaration->declarator, declaration->type_specifier->name) != 0)
    {
        tmidl_diagnostic_t diagnostic;
        diagnostic.level = TMIDL_LEVEL_WARNING;
        diagnostic.message = "The type specifier name must be the same as the declarator.";

        long position = declaration->type_specifier->name_position;
        diagnostic.position_start = position;
        diagnostic.position_end = position + (uint32_t)strlen(declaration->type_specifier->name);

        callbacks->on_diagnostic(&diagnostic, user_context);
    }

    return success;
}

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    // Parse the input
    mpc_parser_t *parser = api_file_parser();
    mpc_result_t r;
    bool success = mpc_parse("input", input, parser, &r);
    mpc_cleanup(1, parser);

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
            c_item_declaration_t *item_declaration = (c_item_declaration_t *)item;
            c_declaration_t *c_declaration = item_declaration->declaration;

            bool is_interface = c_declaration->type_specifier->declarations != NULL;

            if (!validate_declaration(c_declaration, callbacks, user_context))
            {
                continue;
            }

            tmidl_declaration_t declaration;
            declaration.type = is_interface ? ITEM_INTERFACE : ITEM_OPAQUE;
            declaration.name = c_declaration->declarator;
            declaration.doc = c_declaration->doc;
            declaration.functions = NULL;
            declaration.functions_count = 0;

            if (is_interface)
            {
                declaration.functions_count = c_declaration->type_specifier->declarations->count;
                char **declarations = c_declaration->type_specifier->declarations->ptr;
                declaration.functions = malloc(sizeof(size_t) * declaration.functions_count);

                for (int i = 0; i < declaration.functions_count; i++)
                {
                    declaration.functions[i] = malloc(sizeof(tmidl_function_t));
                    declaration.functions[i]->name = declarations[i];
                }
            }

            callbacks->on_declaration(&declaration, user_context);

            // Clean up
            if (declaration.functions != NULL)
            {
                for (int i = 0; i < declaration.functions_count; i++)
                {
                    free(declaration.functions[i]);
                }
                free(declaration.functions);
            }
        }
    }

    // Clean up the parsed items
    free_items(array);

    return true;
}
