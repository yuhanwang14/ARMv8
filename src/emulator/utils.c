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
        fprintf(fd, "X%02d    = %016" PRIu64 "\n", i, reg->g_reg[i]);
    }
    fprintf(fd, "PSTATE: %c%c%c%c", flag_conv(N, 'N'), flag_conv(Z, 'N'), flag_conv(C, 'N'),
            flag_conv(V, 'N'));
    fprintf(fd, "Non-zero memory:\n");
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (reg->ram[i] != 0) {
            fprintf(fd, "0x%08d: 0x%08" PRIu32 "\n", i, reg->ram[i]);
        }
    }
}
