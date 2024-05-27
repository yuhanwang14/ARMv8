#include "register.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Register *reg_init(void) {
    Register *result = malloc(sizeof(Register));
    result->PC = 0;
    PState *PSTATE = malloc(sizeof(PState));
    PSTATE->N = 0;
    PSTATE->Z = 1;
    PSTATE->C = 0;
    PSTATE->V = 0;
    result->PSTATE = PSTATE;
    memset(result->g_reg, 0, 31 * sizeof(uint64_t));
    memset(result->ram, 0, MEMORY_SIZE * sizeof(uint32_t));
    return result;
}

void reg_free(Register *reg) {
    if (reg != NULL) {
        free(reg->PSTATE);
        free(reg);
    }
}
