#include "fetch.h"
#include "utils.h"
#include <features.h>
#include <stdint.h>
#include <stdio.h>

void fail_PC_out_of_bound(void) {
    fprintf(stderr, "PC out of bound");
    exit(EXIT_FAILURE);
}

uint32_t fetch(Register *reg) {
    if (reg->PC >= MEMORY_SIZE) {
        // PC is out of the memory bound
        fail_PC_out_of_bound();
    }

    uint32_t instruction = *(uint32_t *)(reg->ram + reg->PC);
    printf("\n");
    printf("PC: %lu\n", reg->PC);
#if __GNUC__ == 14
    printf("instruction: %032b\n", instruction);
#else
    printf("instruction: %x\n", instruction);
#endif
    // find the adress of instruction
    return instruction;
}
