#include "decode.h"
#include "execute.h"
#include "fetch.h"
#include "instr.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static FILE *out, *source;
static Register *reg;
static Instr *instr;
static const uint32_t HALT = 0x8a000000;

int main(int argc, char **argv) {
    // process arguments
    if (argc == 2) {
        // output file not specified, print to stdout
        out = stdout;
    } else if (argc == 3) {
        // print to given file
        out = safe_open(argv[2], "w+");
    } else {
        puts("usage: emulate <binary> <log file>");
        return EXIT_FAILURE;
    }

    // initialize register machine and load the program
    source = safe_open(argv[1], "r");
    reg = reg_init();
    instr = malloc(sizeof(Instr));
    fread(reg->ram, sizeof(uint8_t), BYTE_COUNT, source);
    if (ferror(source)) {
        fprintf(stderr, "Error encountered when reading from %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    // main loop
    while (true) {
        uint32_t code = fetch(reg);
        if (code == HALT) {
            break;
        }
        decode(code, instr);
        execute(reg, instr);
    }
    log_state(reg, out);

    // clean up after ourselves
    free(instr);
    reg_free(reg);
    fclose(out);
    fclose(source);
    return EXIT_SUCCESS;
}
