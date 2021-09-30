#include <tmidl.h>

#include "types.h"
#include "parser/api_file.h"
#include "parser/mpc_utils.h"
#include "parser/c_declaration.h"

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
        diagnostic.position = r.error->state.pos;
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

            tmidl_declaration_t declaration;
            declaration.type = ITEM_OPAQUE;
            declaration.name = item_declaration->declaration->declarator;
            declaration.doc = item_declaration->declaration->doc;
            declaration.functions = NULL;
            declaration.functions_count = 0;

            callbacks->on_declaration(&declaration, user_context);
        }
    }

    // Clean up the parsed items
    free_items(array);

    return true;
}
