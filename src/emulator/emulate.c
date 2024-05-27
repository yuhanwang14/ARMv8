#include "decode.h"
#include "execute.h"
#include "fetch.h"
#include "instr.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

FILE *out, *source;
Register *reg;
Instr *instr;
const uint32_t HALT = 0x8a000000;

int main(int argc, char **argv) {
    if (argc == 2) {
        // print to stdout
        out = stdout;
    } else if (argc == 3) {
        // print to given file
        out = safe_open(argv[2], "w+");
    } else {
        puts("usage: emulate <binary> <log file>");
        return EXIT_FAILURE;
    }
    source = safe_open(argv[1], "r");
    reg = reg_init();
    fread(reg->ram, sizeof(uint32_t), MEMORY_SIZE, source);

    while (true) {
        uint32_t code = fetch(reg);
        if (code == HALT) {
            break;
        }
        instr = decode(code);
        execute(reg, instr);

        free(instr);
        log_state(reg, out);
    }
    log_state(reg, out);

    fclose(out);
    fclose(source);
    return EXIT_SUCCESS;
}
