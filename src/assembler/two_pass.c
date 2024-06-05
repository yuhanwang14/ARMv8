#include "two_pass.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *input_path;
static unsigned line_count = 0;
static Map *labels;

// in practice it should be much bigger
static const size_t MAP_INITIAL_SIZE = 2;
static const size_t LINE_BUF_SIZE = 128;
static const size_t LABEL_LENGTH = 16;

static Map *map_new(int initial_size) {
    Map *result = malloc(sizeof(Map));
    result->pairs = malloc(sizeof(Pair) * initial_size);
    result->used = 0;
    result->size = initial_size;

    return result;
}

static void map_add(Map *table, char *k, int v) {
    Pair *pair = malloc(sizeof(Pair));
    pair->key = k;
    pair->value = v;
    if (table->used >= table->size) {
        table->size *= 2;
        table->pairs = realloc(table->pairs, table->size * sizeof(Pair));
    }

    table->pairs[table->used] = pair;
    table->used++;
}

static int map_find(Map *table, const char *key) {
    for (int i = 0; i < table->used; i++) {
        if (strcmp(key, table->pairs[i]->key) == 0) {
            return table->pairs[i]->value;
        }
    }
    return -1;
}

void set_input(char *path) { input_path = path; }

unsigned first_pass() {
    FILE *f = safe_open(input_path, "r");
    int c = fgetc(f);
    unsigned line_len = 0;
    char *buf = malloc(sizeof(char) * LINE_BUF_SIZE);
    labels = map_new(MAP_INITIAL_SIZE);

    while (c != EOF) {
        if (c == '\n') {
            // omit empty lines
            if (line_len > 0) {
                line_count++;
            }
            // reset line_len
            line_len = 0;
        } else if (c == ':') {
            // add to map
            char *label = malloc(sizeof(char) * (line_len + 1));
            strncpy(label, buf, line_len);
            label[line_len] = '\0';
            map_add(labels, label, line_count);
            // don't count label declarations
            line_count--;
        } else {
            // record c
            buf[line_len++] = c;
        }
        c = fgetc(f);
    }

    free(buf);
    fclose(f);
    return line_count;
}

void read_into(char **buf) {
    FILE *f = safe_open(input_path, "r");
    int c = fgetc(f);
    char *line_buf = malloc(sizeof(char) * LINE_BUF_SIZE);
    unsigned line_len = 0;
    unsigned current_line = 0;
    bool is_label = false;

    while (c != EOF) {
        if (c == '\n') {
            if (line_len > 0) {
                if (!is_label) {
                    // record line
                    char *line = malloc(sizeof(char) * (line_len + 1 + LABEL_LENGTH));
                    strncpy(line, line_buf, line_len);
                    line[line_len] = '\0';
                    buf[current_line] = line;
                }
                current_line++;
            }
            // reset line state
            is_label = false;
            line_len = 0;
        } else if (c == ':') {
            // don't record this line
            is_label = true;
            current_line--;
        } else if (c == ' ' && line_len == 0) {
            // skip
        } else {
            // record c
            line_buf[line_len++] = c;
        }
        c = fgetc(f);
    }

    free(line_buf);
    fclose(f);
}

static int last_arg(char *line) {
    int res = -1;
    for (int i = strlen(line) - 1; i >= 0; i--) {
        if (line[i] == ' ' || line[i] == ',') {
            res = i + 1;
            break;
        }
    }
    return res;
}

static int find_label(char *line) {
    int index = -1;
    if (strncmp("b.", line, 2) == 0) {
        index = last_arg(line);
    } else if (strncmp("b ", line, 2) == 0) {
        index = 2;
    } else if (strncmp("ldr ", line, 4) == 0
               // not a register
               && line[strlen(line) - 1] != ']') {
        index = last_arg(line);
        if (line[index] == '#') {
            // not a label
            index = -1;
        }
    }
    return index;
}

void substitute_labels(char **lines) {
    for (int i = 0; i < line_count; i++) {
        char *line = lines[i];
        int label_start = find_label(line);
        if (label_start == -1) {
            // does not contain a label
            continue;
        }
        size_t label_len = strlen(line) - label_start;
        char *label = malloc(sizeof(char) * (label_len + 1));
        strncpy(label, line + label_start, label_len);
        label[label_len] = '\0';

        line[label_start] = '#';
        sprintf(line + label_start + 1, "%u", map_find(labels, label));
        free(label);
    }

    // free the map
    for (int i = 0; i < labels->used; i++) {
        free(labels->pairs[i]->key);
        free(labels->pairs[i]);
    }
    free(labels->pairs);
    free(labels);
}
