#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *key; // label
    int value;       // corresponding offset
} Pair;

typedef struct {
    Pair *pairs; // list of "key & value pairs"
    int used;    // current no. of pairs
    int size;    // capacity of the list
} Map;

typedef struct {
    char **lines;
    int length;
} FileLines;

FileLines *two_pass(char *file_name);

void free_file_lines(FileLines *file_lines);
