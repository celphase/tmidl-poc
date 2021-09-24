#include "auxil.h"

#include <stdio.h>

int auxil_getchar(pcc_auxil_o *auxil) {
    return auxil->callbacks->next_codepoint(auxil->context);
}

void auxil_error(pcc_auxil_o *auxil) {
    auxil->success = false;
}
