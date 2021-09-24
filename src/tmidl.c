#include <tmidl.h>

#include <stdio.h>

#include "auxil.h"
#include "generated.h"

bool parse_tmidl() {
    pcc_auxil_o auxil = auxil_create();

    pcc_context_t *ctx = pcc_create(&auxil);
    while (pcc_parse(ctx, NULL));
    pcc_destroy(ctx);

    return auxil.success;
}
