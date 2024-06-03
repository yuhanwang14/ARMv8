#include "two_pass.h"
#include <assert.h>

static const int INITIAL_SIZE = 8;
static const int INITIAL_LINE_SIZE = 32;

// final output
static char **LINES;
static Map *TABLE;
static int LENGTH;

// creating a table for mapping labels

// check whether table is null
static void check_null(Map *table) {
    if (table == NULL) {
        printf("Table is NULL\n");
        assert(table != NULL);
    }
}

static Map *map_new(int initial_size) {
    Map *result = malloc(sizeof(Map));
    result->pairs = malloc(sizeof(Pair) * initial_size);
    result->used = 0;
    result->size = initial_size;

    return result;
}

void map_add(Map *table, const char *k, int v) {
    check_null(table);

    Pair *pair = malloc(sizeof(Pair));
    pair->key = k;
    pair->value = v;
    // resize if no. of element exceed the size of array
    if (table->used >= table->size) {
        table->pairs = realloc(table->pairs, table->size * 2 * sizeof(Pair));

        table->size *= 2;
    }

    table->pairs[table->used] = *pair;
    table->used++;
}

int map_find(Map *table, const char *key) {
    check_null(table);

    for (int i = 0; i < table->used; i++) {
        if (strcmp(key, table->pairs[i].key) == 0) {
            return table->pairs[i].value;
        }
    }

    return -1;
}

// returns the substr of str from start index to end
char *sub_str(char *str, int start, int end) {
    char *sub = malloc((end - start + 2) * sizeof(char));
    memcpy(sub, &str[start], end - start + 1);
    sub[end - start + 1] = '\0';

    return sub;
}

// don't count label line
int count_non_empty_lines(char *file_name) {
    int lines = 0;
    int count = 0; // counting \n

    FILE *fptr = fopen(file_name, "r");
    int c = fgetc(fptr);
    while (c != EOF) {
        if (c == ':') {
            lines--;
        } // label line does not count

        if (c == '\n' && count > 0) { // nonempty line, count increment by one
            lines++;
            count = 0;
        } else if (c == '\n' && count == 0) {
            // do nothing if empty line
        } else {
            count++;
        }

        c = fgetc(fptr);
    }

    fclose(fptr);

    return lines;
}

// first pass
void split_lines(char *file_name) {
    int i = 0;
    int line = 0;
    int total_lines = count_non_empty_lines(file_name);
    char **lines = malloc(total_lines * sizeof(char *));

    FILE *fptr = fopen(file_name, "r");
    int c = fgetc(fptr);

    Map *table = map_new(INITIAL_SIZE);

    while (c != EOF) {
        if (c == '\n') { // ignore empty line
            c = fgetc(fptr);
            continue;
        }

        lines[line] = malloc(INITIAL_LINE_SIZE * sizeof(char));

        // if not new line or end of file, keep adding content to the current line
        while (c != '\n' && c != EOF) {
            if (i > 0 || c != ' ') { // remove tab
                lines[line][i] = (char)c;
                i++;
            }

            if (c == ':') { // find label
                break;
            }

            c = fgetc(fptr);
        }

        if (c == ':') { // add label
            size_t len = strlen(lines[line]) - 1;
            char *label = malloc(len * sizeof(char));
            strncpy(label, lines[line], len);

            map_add(table, label, line);
            line--; // doesn't count the label line
            c = fgetc(fptr);
            while (c == ' ') { // remove all spaces after ':'
                c = fgetc(fptr);
            }
        }

        line++;
        i = 0;
        c = fgetc(fptr);
    }

    fclose(fptr);

    TABLE = table;
    LINES = lines;
    LENGTH = total_lines;
}


// helper function for finding the position of the literal
int return_index(char *line) {
    int res = -1;
    int ind = strlen(line);

    for (int i = ind - 1; i >= 0; i--) {
        if (line[i] == ' ' || line[i] == ',') {
            res = i + 1;
            break;
        }
    }

    return res;
}

// returns the position of the (potential) label
int find_literal(char *line) {
    int res = -1;
    if (strncmp("b.", line, 2) == 0) {
        res = return_index(line);
    } else if (strncmp("b ", line, 2) == 0) {
        res = 2;
    } else if (strncmp("ldr ", line, 4) == 0) {
        res = return_index(line);

        if (line[res] == '#') {
            res = -1;
        }
    }

    return res; // returns the starting index of the label
}

// two pass
FileLines *two_pass(char *file_name) {
    // first pass
    split_lines(file_name);

    // second pass
    for (int i = 0; i < LENGTH; i++) { // i + 1 = line number
        char *line = LINES[i];
        int start = find_literal(line);

        if (start == -1) { // if label empty, check next item
            continue;
        }

        int last = strlen(line) - 1;
        char *label = sub_str(line, start, last);

        int ind = map_find(TABLE, label);

        if (ind == -1) { // if not label, check next item
            continue;
        }

        char *offset = malloc(sizeof(char *));

        assert(offset != NULL); // check malloc is working

        sprintf(offset, "%d", ind);

        char *new_str;
        new_str = sub_str(line, 0, start - 1);
        strcat(new_str, "#");
        strcat(new_str, offset);

        LINES[i] = new_str;

        free(offset);
    }

    FileLines *two_passed = malloc(sizeof(FileLines));
    two_passed->lines = malloc(LENGTH * sizeof(char *));
    two_passed->lines = LINES;
    two_passed->length = LENGTH;

    return two_passed;
}

void free_file_lines(FileLines *file_lines) {
    free(file_lines->lines);
    free(file_lines);
}
