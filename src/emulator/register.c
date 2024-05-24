#include "register.h"
#include <stdlib.h>

Register *reg_init() {
    Register *result = malloc(sizeof(Register));
    result->PC = 0;
    PState *PSTATE = malloc(sizeof(PState));
    PSTATE->N = 0;
    PSTATE->Z = 1;
    PSTATE->C = 0;
    PSTATE->V = 0;
    result->PSTATE = PSTATE;
    return result;
}

void reg_free(Register *reg) {
    if (reg != NULL) {
        free(reg->PSTATE);
        free(reg);
    }
}
