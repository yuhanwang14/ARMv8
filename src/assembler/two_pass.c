#include "two_pass.h"
#include <assert.h>

const int INITIAL_SIZE = 4;
const int INITIAL_LINE_SIZE = 32;

// creating a table for mapping labels
Index *index_new(int initial_size) {
    Index *result = malloc(sizeof(Index));
    result->pairs = malloc(sizeof(Pair) * initial_size);
    result->used = 0;
    result->size = initial_size;

    return result;
}

void index_add(Index *index, const char *k, int v) {
    Pair *pair = malloc(sizeof(Pair));
    pair->key = k;
    pair->value = v;
    // resize if no. of element exceed the size of array
    if (index->used >= index->size) {
        index->pairs = realloc(index->pairs, index->size * 2 * sizeof(Pair));

        index->size *= 2;
    }

    index->pairs[index->used] = *pair;
    index->used++;
}

int index_find(Index *index, const char *key) {
    for (int i = 0; i < index->used; i++) {
        if (strcmp(key, index->pairs[i].key) == 0) {
            return index->pairs[i].value;
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

int count_lines(char *file_name) {
    int lines = 1;

    FILE *fptr = fopen(file_name, "r");
    int c = fgetc(fptr);
    while (c != EOF) {
        if (c == '\n') {
            lines++;
        }
        c = fgetc(fptr);
    }
    fclose(fptr);

    return lines;
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
        }

        if (c == '\n' && count > 0) {
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

// delete label line (label:)
FirstPass *split_lines(char *file_name) {
    int i = 0;
    int line = 0;                                       // 14
    int total_lines = count_non_empty_lines(file_name); // 11
    char **lines = malloc(total_lines * sizeof(char *));

    FILE *fptr = fopen(file_name, "r");
    int c = fgetc(fptr);

    Index *table = index_new(INITIAL_SIZE);

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

            if (c == ':') {
                break;
            }

            c = fgetc(fptr);
        }

        if (c == ':') { // add label
            size_t len = strlen(lines[line]) - 1;
            char *label = malloc(len * sizeof(char));
            strncpy(label, lines[line], len);

            index_add(table, label, line);
            line--;
            c = fgetc(fptr);
            while (c == ' ') {
                c = fgetc(fptr);
            }
        }

        line++;
        i = 0;
        c = fgetc(fptr);
        //        free(lines[line]); // free the memory
    }

    fclose(fptr);

    FirstPass *first_pass = malloc(sizeof(FirstPass));
    first_pass->table = table;
    first_pass->lines = malloc(total_lines * sizeof(char *));
    first_pass->lines = lines;
    first_pass->length = total_lines;

    return first_pass;
}

FileLines *two_pass(char *file_name) {
    // first pass
    FirstPass *first_pass = split_lines(file_name);

    // second pass
    for (int i = 0; i < first_pass->length; i++) { // i + 1 = line number
        char *line = first_pass->lines[i];
        int start = find_label(line);

        if (start == -1) { // if label empty, check next item
            continue;
        }

        int last = strlen(line) - 1;
        char *label = sub_str(line, start, last);

        int ind = index_find(first_pass->table, label);

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

        first_pass->lines[i] = new_str;

        //        free(offset);
    }

    FileLines *two_passed = malloc(sizeof(FileLines));
    two_passed->lines = malloc(first_pass->length * sizeof(char *));
    two_passed->lines = first_pass->lines;
    two_passed->length = first_pass->length;

    return two_passed;
}

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

int find_label(char *line) { // returns the position of the (potential) label
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
