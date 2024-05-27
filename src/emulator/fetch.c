#include "fetch.h"
#include "utils.h"
#include <stdint.h>

void fail_PC_out_of_bound(void) {
    fprintf(stderr, "PC out of bound");
    exit(EXIT_FAILURE);
}

uint32_t fetch(Register *reg) {
    if (reg->PC >= MEMORY_SIZE) {
        // PC is out of the memory bound
        fail_PC_out_of_bound();
    }

    uint32_t instruction = *(uint32_t *)(reg->ram + reg->PC / sizeof(uint32_t));
    // find the adress of instruction
    reg->PC++;
    // update the program counter
    return instruction;
}
