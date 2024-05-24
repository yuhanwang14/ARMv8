#include "fs.h"
#include <stdio.h>
#include <stdlib.h>

void panic(const char *reason) {
    perror(reason);
    exit(EXIT_FAILURE);
}

FILE *safe_open(char *path) {
    FILE *result = fopen(path, "w+");
    if (result == NULL) {
        panic("Failed to open output file for writing");
    }
    return result;
}
