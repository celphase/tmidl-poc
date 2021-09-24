#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int (*next_codepoint)(void *context);
} tmidl_callbacks_i;

bool parse_tmidl(const tmidl_callbacks_i *callbacks, void *context);

#ifdef __cplusplus
}
#endif
