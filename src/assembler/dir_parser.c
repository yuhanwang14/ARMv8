#include "dir_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint32_t parse_dir(char *opcode, char **arguments) {
    if (strcmp(opcode, ".int") == 0) {
        return strtol(arguments[0], NULL, 0);
    } else {
        fprintf(stderr, "failed to parse '%s' as directive\n", opcode);
        exit(EXIT_FAILURE);
    }
}
