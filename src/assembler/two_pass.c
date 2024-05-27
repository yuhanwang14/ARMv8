#include "two_pass.h"
#include <string.h>

const size_t INITIAL_SIZE = 4;

extern int find_label(char *line);

// creating a table for mapping labels

Index *index_new(void) {
    Index *result = malloc(sizeof(Index));
    result->pairs = malloc(sizeof(Pair) * INITIAL_SIZE);
    result->used = 0;
    result->size = INITIAL_SIZE;

    return result;
}

void index_add(Index *index, const char *k, int v) {
    Pair *pair = malloc(sizeof(Pair));
    pair->key = k;
    pair->value = v;

    // resize if no. of element exceed the size of array
    if (index->used >= index->size) { 
        index->pairs = realloc(index->pairs, index->size*2);
    }

    index->pairs[index->used] = *pair;
    // printf("Added (%s, %d)", *k, v);
}

int index_find(Index *index, const char *key) {
    for (int i = 0; i< index->used; i++) {
        if (strcmp(key, index->pairs[i].key) == 0) {
            return index->pairs[i].value;
        }
    }

    // return -1 if not found
    return -1;
}

// implement two-pass

char *sub_str(char *str, int start, int end) { // returns the substr of str from start index to end
    char *sub = malloc((end - start + 2) * sizeof(char));
    memcpy(sub, &str[start], end - start + 1);
    sub[end - start + 1] = '\0';

    return sub;
}

void line_check_label(Index *index, char *line, int address) {
    if (strchr(line, ':')) {
        size_t len = strlen(line);
        char *label = malloc(len * sizeof(char));
        strncpy(label, line, len - 1);
        label[len - 1] = '\0';

        index_add(index, label, address);
    }
}

int count_lines(FILE *file) {
    int lines = 1;
    while (!feof(file)) {
        char c = fgetc(file);
        if (c == '\n') {
            lines++;
        }
    }

    return lines;
}

char **split_lines(FILE *file) {
    int start = 0;
    int end = 0;
    int line_number = 0;
    int total_lines = count_lines(file);
    char *line; // need to resize
    char **lines = malloc(total_lines * sizeof(char *));

    while (!feof(file)) {
        char c = fgetc(file);
        int count = 0;

        // remove tab
        while (c != '\n' && c != EOF) { 
            if (count != 0 || c != ' ') {
                line[end] = c;
                count++;
            } 

            end++;
            char c = fgetc(file);
        }

        start = end;
        lines[line_number] = line;
        line_number++;
    }

    return lines;
}

Index *first_pass(char **lines, int number_of_lines) {
    Index *table = index_new();
    for (int i = 0; i < number_of_lines; i++) { // i + 1 = line number
        line_check_label(table, lines[i], (i + 1) * 4);
    }

    return table;
}

// values: integers

char **two_pass(FILE *file) {
    // add # in front of label
    int number_of_lines = count_lines(file);
    char **lines = split_lines(file);
    Index *table = first_pass(lines, number_of_lines);
    int line_number = 0;

    for (int i = 0; i < number_of_lines; i++) { // i + 1 = line number
        int start = find_label(lines[i]);
        if (start != -1) { // if label not empty, replace string
            char *label = sub_str(lines[i], start, strlen(lines[i] - 1));
            int len = strlen(label); 
        
            char *offset;
            sprintf(offset, "%d", i + 1 - index_find(table, label)); 
            char *new_str;
            new_str = sub_str(lines[i], 0, start - 1);
            strcat(new_str, offset);
            strncpy(new_str, lines[i], strlen(offset) + start);
            lines[i] = new_str;
        }

        
    }

    return lines;
}

int find_label(char *line) {
    int res = -1;
    if (!strncmp("br ", line, 3)) {
        res = 3;
    } else if (!strncmp("b.", line, 2)) {
        res = 2;
    } else if (!strncmp("b ", line, 2)) {
        res = 2;
    } else if (!strncmp("ldr ", line, 4)) {
        res = 4;
        strtok(line, " ");
        char *new_str = strtok(NULL, " ");
        res += strlen(new_str) + 1;
    }
    
    return res; // returns the starting index of the label
}

