#include "fetch.h"
#include "utils.h"
#include <features.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t fetch(Register *reg) {
    if (reg->PC >= BYTE_COUNT) {
        // reading outside of PC, this is no good
        fprintf(stderr, "\n");
        fprintf(stderr, "PC out of bound\n");
        fprintf(stderr, "Expected PC to be less than 0x%x, but is 0x%lx\n", BYTE_COUNT, reg->PC);
        exit(EXIT_FAILURE);
    }

    // read the corresponding word
    uint32_t instruction = *(uint32_t *)(reg->ram + reg->PC);
    return instruction;
}
