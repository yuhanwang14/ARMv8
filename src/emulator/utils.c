#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void fail_open_file(const char *path) {
    fprintf(stderr, "Failed to open %s for writing", path);
    exit(EXIT_FAILURE);
}

FILE *safe_open(char *path) {
    FILE *result = fopen(path, "w+");
    if (result == NULL) {
        fail_open_file(path);
    }
    return result;
}
