#include "parser.h"
#include "two_pass.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *out;
static const unsigned INSTR_SIZE = 4;

int main(int argc, char **argv) {
    if (argc == 2) {
        // print to stdout
        out = stdout;
    } else if (argc == 3) {
        // print to given file
        out = safe_open(argv[2], "w");
    } else {
        puts("usage: assemble <.s file> <binary file>");
        return EXIT_FAILURE;
    }
    // opens assembly source file
    set_input(argv[1]);
    unsigned line_count = first_pass();
    char **lines = malloc(line_count * sizeof(char *));
    read_into(lines);
    substitute_labels(lines);

    for (int i = 0; i < line_count; i++) {
        // parse each line
	printf("lines[%d]: %s\n", i, lines[i]);
        uint32_t result = parse_instruction(lines[i], i);
        // immediatly write to output
        fwrite(&result, INSTR_SIZE, 1, out);
    }

    // clean up
    fclose(out);
    for (unsigned i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    return EXIT_SUCCESS;
}
