#include <stdio.h>
#include <stdlib.h>
#include "two_pass.h"
#include "parser.h"

int main(int argc, char **argv) {
    FILE *fptr;
    FileLines *file_lines = two_pass(fptr);
    for (int i = 0; i < file_lines->length; i++) {
        uint32_t encode = parse_instruction(file_lines->lines[i]);
        char *str;
        fprintf(fptr, "%d", encode);
    }

    return EXIT_SUCCESS;
}
