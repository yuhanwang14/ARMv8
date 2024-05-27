#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    const char *key; // label
    int value; // corresponding offset
} Pair;

typedef struct {
    Pair *pairs;
    int used;
    int size;
} Index;

Index *index_new(void);

void index_add(Index *index, const char *key, int value);

int index_find(Index *index, const char *key);
