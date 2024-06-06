#include "two_pass.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *input_path;
static unsigned line_count = 0;
static Map *labels;

// in practice these should be much bigger
static const size_t MAP_INITIAL_SIZE = 2;
static const size_t LINE_BUF_SIZE = 4;
// we limit the literal for a label to be an int, the corresponding string can only be this long
static const size_t MAX_LABEL_LENGTH = 16;

// creates an empty line
static Map *map_new(int initial_size) {
    Map *result = malloc(sizeof(Map));
    result->pairs = malloc(sizeof(Pair) * initial_size);
    result->used = 0;
    result->size = initial_size;

    return result;
}

// adds item to map
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

// looks for a label in given map
static int map_find(Map *table, const char *key) {
    for (int i = 0; i < table->used; i++) {
        if (strcmp(key, table->pairs[i]->key) == 0) {
            return table->pairs[i]->value;
        }
    }
    return -1;
}

// sets a local variable so we don't have to pass it every time
void set_input(char *path) { input_path = path; }

// counts the lines and collects the labels
unsigned first_pass() {
    FILE *f = safe_open(input_path, "r");
    char *buf = malloc(sizeof(char) * LINE_BUF_SIZE);
    labels = map_new(MAP_INITIAL_SIZE);
    unsigned line_len = 0;
    size_t bufsize = LINE_BUF_SIZE;
    int c = fgetc(f);

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
        } else if (c == ';') {
            // handle comments
            while (c != '\n'){
                c = fgetc(f);
            }
            if (line_len > 0) {
                line_count++;
            }
        }else{
            // record c
            if (bufsize <= line_len) {
                // resize the buffer
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[line_len++] = c;
        }
        c = fgetc(f);
    }

    // clean up
    free(buf);
    fclose(f);
    return line_count;
}

void read_into(char **buf) {
    FILE *f = safe_open(input_path, "r");
    char *line_buf = malloc(sizeof(char) * LINE_BUF_SIZE);
    unsigned line_len = 0;
    unsigned current_line = 0;
    size_t bufsize = LINE_BUF_SIZE;
    bool is_label = false;
    int c = fgetc(f);

    while (c != EOF) {
        if (c == '\n') {
            if (line_len > 0) {
                if (!is_label) {
                    // record line
                    char *line = malloc(sizeof(char) * (line_len + 1 + MAX_LABEL_LENGTH));
                    strncpy(line, line_buf, line_len);
                    line[line_len] = '\0';
                    buf[current_line] = line;
                }
                // increment line count
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
            // skip spaces
        } else if (c == ';') {
            // handle comments
            while (c != '\n'){
                c = fgetc(f);
            }
            if (line_len > 0) {
                if (!is_label) {
                    // record line
                    char *line = malloc(sizeof(char) * (line_len + 1 + MAX_LABEL_LENGTH));
                    strncpy(line, line_buf, line_len);
                    line[line_len] = '\0';
                    buf[current_line] = line;
                }
                // increment line count
                current_line++;
            }
        } else {
            // record c
            if (bufsize <= line_len) {
                // resize the buffer
                bufsize *= 2;
                line_buf = realloc(line_buf, bufsize);
            }
            line_buf[line_len++] = c;
        }
        c = fgetc(f);
    }

    // clean up
    free(line_buf);
    fclose(f);
}

// finds the index of the last argument, identified by a space or a comma
static int last_arg(char *line) {
    // search backwards
    for (int i = strlen(line) - 1; i >= 0; i--) {
        if (line[i] == ' ' || line[i] == ',') {
            return i + 1;
        }
    }
    return -1;
}

// finds the index of label
static int find_label(char *line) {
    int index = -1;
    if (strncmp("b.", line, 2) == 0) {
        // conditional branch
        index = last_arg(line);
    } else if (strncmp("b ", line, 2) == 0) {
        // unconditional branch
        index = 2;
    } else if (strncmp("ldr ", line, 4) == 0
               // not a register
               && line[strlen(line) - 1] != ']') {
        // load operation
        index = last_arg(line);
        if (line[index] == '#') {
            // a literal, not a label
            index = -1;
        }
    }
    return index;
}

// substitute all the labels into literals
void substitute_labels(char **lines) {
    for (int i = 0; i < line_count; i++) {
        char *line = lines[i];
        int label_start = find_label(line);
        if (label_start == -1) {
            // does not contain a label
            continue;
        }
        // read out the label
        size_t label_len = strlen(line) - label_start;
        char *label = malloc(sizeof(char) * (label_len + 1));
        strncpy(label, line + label_start, label_len);
        label[label_len] = '\0';

        // do the substitution
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
