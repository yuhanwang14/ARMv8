#include "fs.h"
#include <stdio.h>
#include <stdlib.h>

FILE *out;

int main(int argc, char **argv) {
    if (argc == 2) {
        // print to stdout
        out = stdout;
    } else if (argc == 3) {
        // print to given file
        out = safe_open(argv[2]);
    }
    return EXIT_SUCCESS;
}
