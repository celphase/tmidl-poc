#pragma once

#include <tmidl.h>
#include <stdbool.h>

typedef struct {
    bool success;
    const tmidl_callbacks_i *callbacks;
    void *context;
} pcc_auxil_o;

int auxil_getchar(pcc_auxil_o *auxil);

void auxil_error(pcc_auxil_o *auxil);
