#include <tmidl.h>

#include "types.h"
#include "parser/api_file.h"
#include "parser/mpc_utils.h"

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context)
{
    // Parse the input
    mpc_result_t r;
    bool success = mpc_parse("input", input, api_file(), &r);

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
    api_item_t **items = array->ptr;

    printf("A\n");
    for (int i = 0; i < array->count; i++)
    {
        callbacks->on_item(items[i], user_context);
    }

    // Clean up the parsed items
    free_items(array);

    return true;
}
