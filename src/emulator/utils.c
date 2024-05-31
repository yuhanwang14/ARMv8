#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

FILE *safe_open(char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (f == NULL) {
        fprintf(stderr, "Failed to open %s with mode %s", path, mode);
        exit(EXIT_FAILURE);
    }
    return f;
}

// gives the string representation of whether a flag is set
#define show_flag(ident) reg->PSTATE->ident ? #ident : "-"

void log_state(Register *reg, FILE *fd) {
    // log register state
    fprintf(fd, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        // we want to print the reg id as 2-long decimal and the contents a 16-long hex
        fprintf(fd, "X%02d    = %016lx\n", i, reg->g_reg[i]);
    }

    // log PC and PSTATE
    fprintf(fd, "PC     = %016lx\n", reg->PC);
    fprintf(fd, "PSTATE : %s%s%s%s\n", show_flag(N), show_flag(Z), show_flag(C), show_flag(V));

    // log non-zero memory
    fprintf(fd, "Non-Zero Memory:\n");
    uint32_t *ram = (uint32_t *)reg->ram;
    for (int i = 0; i < BYTE_COUNT / sizeof(uint32_t); i++) {
        if (ram[i] != 0) {
            fprintf(fd, "0x%08lx : %08x\n", i * sizeof(uint32_t), ram[i]);
        }
    }
}

// https://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
int64_t sign_extend(int64_t x, unsigned nbits) {
    const int64_t m = 1U << (nbits - 1);
    x = x & ((1U << nbits) - 1);
    return (x ^ m) - m;
}
