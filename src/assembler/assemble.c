#include "parser.h"
#include "two_pass.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *out, *source;
static const unsigned INSTR_SIZE = 4;

static FILE *safe_open(char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (f == NULL) {
        fprintf(stderr, "Failed to open %s with mode %s", path, mode);
        exit(EXIT_FAILURE);
    }
    return f;
}

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
    FileLines *file_lines = two_pass(argv[1]);

    // opens assembly source file
    source = safe_open(argv[1], "r");

    for (int i = 0; i < file_lines->length; i++) {
        // parse each line
        uint32_t result = parse_instruction(file_lines->lines[i], i);
        // immediatly write to output
        fwrite(&result, INSTR_SIZE, 1, out);
    }

    // clean up
    fclose(out);
    fclose(source);

    return EXIT_SUCCESS;
}
