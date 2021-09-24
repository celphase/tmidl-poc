#pragma once

#include <stdbool.h>

typedef struct {
    bool success;
} pcc_auxil_o;

pcc_auxil_o auxil_create();

int auxil_getchar(pcc_auxil_o *auxil);

void auxil_error(pcc_auxil_o *auxil);
