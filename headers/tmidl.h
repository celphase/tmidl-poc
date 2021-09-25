#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct api_function_t
{
    char *name;
} api_function_t;

typedef enum api_item_type_t
{
    ITEM_OPAQUE,
    ITEM_INTERFACE
} api_item_type_t;

typedef struct api_item_t
{
    api_item_type_t type;
    char *name;
    char *doc;
    api_function_t **functions;
    int functions_count;
} api_item_t;

typedef struct tmidl_callbacks_i
{
    void (*on_item)(const api_item_t *item, void *user_context);
    void (*on_error)(const char *message, long position, void *user_context);
} tmidl_callbacks_i;

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context);

#ifdef __cplusplus
}
#endif
