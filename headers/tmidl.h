#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct api_item_t
{
    enum Type
    {
        ITEM_OPAQUE,
        ITEM_INTERFACE
    } type;
    char *name;
    char *doc;
} api_item_t;

typedef struct tmidl_callbacks_i {
    void (*on_item)(const api_item_t *item, void *user_context);
    void (*on_error)(const char *message, long position, void *user_context);
} tmidl_callbacks_i;

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context);

#ifdef __cplusplus
}
#endif
