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

typedef struct {
    char **lines;
    int length;
} FileLines;

typedef struct {
    char **lines;
    Index *table;
    int length;
} FirstPass;

FileLines *two_pass(char *file_name);

FirstPass *split_lines(char *file_name);

int find_label(char *line);

char *sub_str(char *str, int start, int end);

int count_non_empty_lines(char *file_name);

Index *first_pass(char **lines, int number_of_lines);
