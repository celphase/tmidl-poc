#include <tmidl.h>

#include <stdio.h>

#include "auxil.h"
#include "generated.h"

bool parse_tmidl(const tmidl_callbacks_i *callbacks, void *context) {
    pcc_auxil_o auxil;
    auxil.success = true;
    auxil.callbacks = callbacks;
    auxil.context = context;

    pcc_context_t *ctx = pcc_create(&auxil);

    int ret = 1;
    while (ret == 1) {
        pcc_parse(ctx, &ret);
    }

    pcc_destroy(ctx);

    return auxil.success;
}
