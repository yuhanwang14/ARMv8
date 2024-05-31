#include "fetch.h"
#include "utils.h"
#include <features.h>
#include <stdint.h>
#include <stdio.h>

uint32_t fetch(Register *reg) {
    if (reg->PC >= WORD_COUNT) {
        // reading outside of PC, this is no good
        fprintf(stderr, "\n");
        fprintf(stderr, "PC out of bound\n");
        fprintf(stderr, "Expected PC to be less than 0x%zx, but is 0x%lx\n", WORD_COUNT, reg->PC);
        exit(EXIT_FAILURE);
    }

    // read the corresponding word
    uint32_t instruction = *(uint32_t *)(reg->ram + reg->PC);
    printf("\n");
    printf("PC: %lu\n", reg->PC);
#if __GNUC__ == 14
    printf("instruction: %032b\n", instruction);
#else
    printf("instruction: %x\n", instruction);
#endif
    return instruction;
}
