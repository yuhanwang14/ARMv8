#include "fetch.h"
#include "utils.h"
#include <features.h>
#include <stdint.h>
#include <stdio.h>

uint32_t fetch(Register *reg) {
    if (reg->PC >= WORD_COUNT) {
        // reading outside of PC, this is no good
        fprintf(stderr, "PC out of bound");
        exit(EXIT_FAILURE);
    }

    // read the corresponding word
    uint32_t instruction = *(uint32_t *)(reg->ram + reg->PC);
    printf("\n");
    printf("PC: %lu\n", reg->PC);
    printf("instruction: %x\n", instruction);
    return instruction;
}
