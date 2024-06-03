#include "parser.h"
#include "two_pass.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *out, *source;

FILE *safe_open(char *path, const char *mode) {
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

    source = safe_open(argv[1], "r");

    for (int i = 0; i < file_lines->length; i++) {
        uint32_t result = parse_instruction(file_lines->lines[i], i);
        fwrite(&result, 4, 1, out);
    }

    fclose(out);
    fclose(source);

    return EXIT_SUCCESS;
}
