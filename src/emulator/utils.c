#include "inttypes.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void fail_open_file(const char *path, const char *mode) {
    fprintf(stderr, "Failed to open %s for %s", path, mode);
    exit(EXIT_FAILURE);
}

FILE *safe_open(char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (f == NULL) {
        fail_open_file(path, mode);
    }
    return f;
}

#define flag_conv(ident, str) reg->PSTATE->ident ? str : '-'

void log_state(Register *reg, FILE *fd) {
    // log register state
    fprintf(fd, "Registers:\n");
    for (int i = 0; i < 31; i++) {
        // PRIu64 is the format string for uint64_t
        // we want to print the reg id as 2-long decimal
        // and the contents a 16-long hex
        fprintf(fd, "X%02d    = %016lx\n", i, reg->g_reg[i]);
    }
    fprintf(fd, "PC     = %016lx\n", reg->PC * sizeof(uint32_t));
    fprintf(fd, "PSTATE : %c%c%c%c\n", flag_conv(N, 'N'), flag_conv(Z, 'Z'), flag_conv(C, 'C'),
            flag_conv(V, 'V'));
    fprintf(fd, "Non-Zero Memory:\n");
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (reg->ram[i] != 0) {
            fprintf(fd, "0x%08lx : %08x\n", i * sizeof(uint32_t), reg->ram[i]);
        }
    }
}
