#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct api_function_t
{
    char *name;
} api_function_t;

typedef enum api_declaration_type_t
{
    ITEM_OPAQUE,
    ITEM_INTERFACE
} api_declaration_type_t;

typedef struct api_declaration_t
{
    api_declaration_type_t type;
    const char *name;
    const char *doc;
    api_function_t **functions;
    size_t functions_count;
} api_declaration_t;

typedef struct tmidl_callbacks_i
{
    void (*on_declaration)(const api_declaration_t *item, void *user_context);
    void (*on_error)(const char *message, long position, void *user_context);
} tmidl_callbacks_i;

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context);

#ifdef __cplusplus
}
#endif
