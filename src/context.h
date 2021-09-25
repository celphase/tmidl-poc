#include "types.h"

typedef struct context_o {
    const tmidl_callbacks_i *callbacks;
    void *user_context;
} context_o;
