#include "auxil.h"

#include <stdio.h>

pcc_auxil_o auxil_create() {
    pcc_auxil_o data;
    data.success = true;
    return data;
}

int auxil_getchar(pcc_auxil_o *auxil) {
    return -1;
}

void auxil_error(pcc_auxil_o *auxil) {
    auxil->success = false;
}
