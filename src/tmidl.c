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
        callbacks->on_error(message, r.error->state.pos, user_context);

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

            api_declaration_t api_item;
            api_item.type = ITEM_OPAQUE;
            api_item.name = item_declaration->declaration->name;
            api_item.doc = item_declaration->declaration->doc;
            api_item.functions = NULL;
            api_item.functions_count = 0;

            callbacks->on_declaration(&api_item, user_context);
        }
    }

    // Clean up the parsed items
    free_items(array);

    return true;
}
