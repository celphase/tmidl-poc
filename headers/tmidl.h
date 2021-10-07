#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tmidl_function_t
{
    char *name;
} tmidl_function_t;

typedef enum tmidl_declaration_type_t {
    ITEM_OPAQUE,
    ITEM_INTERFACE
} tmidl_declaration_type_t;

typedef struct tmidl_declaration_t
{
    tmidl_declaration_type_t type;
    const char *name;
    const char *doc;
    tmidl_function_t **functions;
    size_t functions_count;
} tmidl_declaration_t;

typedef enum tmidl_level_t {
    TMIDL_LEVEL_WARNING,
    TMIDL_LEVEL_ERROR
} tmidl_level_t;

typedef struct tmidl_diagnostic_t
{
    tmidl_level_t level;
    const char *message;
    uint32_t position_start;
    uint32_t position_end;
} tmidl_diagnostic_t;

typedef struct tmidl_callbacks_i
{
    void (*on_declaration)(const tmidl_declaration_t *declaration, void *user_context);
    void (*on_diagnostic)(const tmidl_diagnostic_t *diagnostic, void *user_context);
} tmidl_callbacks_i;

bool parse_tmidl(const char *input, const tmidl_callbacks_i *callbacks, void *user_context);

#ifdef __cplusplus
}
#endif
