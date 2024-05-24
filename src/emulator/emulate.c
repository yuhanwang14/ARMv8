#include "decode.h"
#include "fetch.h"
#include "instr.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

FILE *out, *source;
Register *reg;
Instr *instr;

int main(int argc, char **argv) {
    if (argc == 2) {
        // print to stdout
        out = stdout;
    } else if (argc == 3) {
        // print to given file
        out = safe_open(argv[2]);
    } else {
        puts("usage: emulate <binary> <log file>");
        return EXIT_FAILURE;
    }
    source = safe_open(argv[1]);
    reg = reg_init();

    while (true) {
        uint32_t code = fetch(reg);
        instr = decode(code);
        execute(reg, instr);

        // some housekeeping and reporting the register state
        instr_free(instr);
        log_state(reg, out);
    }

    fclose(out);
    fclose(source);
    return EXIT_SUCCESS;
}
