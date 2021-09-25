#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct item_o
{
    enum Type
    {
        ITEM_OPAQUE,
        ITEM_INTERFACE
    } type;
    char *name;
    char *doc;
} item_o;

typedef struct tmidl_callbacks_i {
    void (*on_item)(const item_o *item, void *user_context);
    void (*on_error)(const char *message, long position, void *user_context);
} tmidl_callbacks_i;

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context);

#ifdef __cplusplus
}
#endif
