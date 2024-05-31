#include <stdlib.h>
#include "two_pass.h"
#include "parser.h"

static FILE *out, *source;

FILE *safe_open(char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (f == NULL) {
        fprintf(stderr, "Failed to open %s with mode %s", path, mode);
        exit(EXIT_FAILURE);
    }
    return f;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        // print to stdout
        out = stdout;
    } else if (argc == 3) {
        // print to given file
        out = safe_open(argv[2], "w+");
    } else {
        puts("usage: emulate <binary> <log file>");
        return EXIT_FAILURE;
    }

    source = safe_open(argv[1], "r");

    FileLines *file_lines = two_pass(source);

    for (int i = 0; i < file_lines->length; i++) {
        uint32_t result = parse_instruction(file_lines->lines[i], i);
        fprintf(source, "%d", result);
    }

    fclose(out);
    fclose(source);

    // debugging

//    char *f1 = "/Users/henrywzh/Desktop/C/armv8_testsuite/test/test_cases/general/dummy_ldr.s";
//    char *f1 = "/Users/henrywzh/Desktop/C/armv8_testsuite/test/test_cases/general/ldr10.s";
//    char *f1 = "/Users/henrywzh/Desktop/C/armv8_testsuite/test/test_cases/branch/b2.s";
//    char *f1 = "/Users/henrywzh/Desktop/C/armv8_testsuite/test/test_cases/general/example.s";

    // test file split lines

//    for (int i = 0; i < no_of_lines; i++) { // print the file by line, ignores empty lines
//        printf("%d: %s\n", i, f1_lines[i]);
////        int x = find_label(f1_lines[i]);
////        if (x != -1) {
////            printf("%d: %s, index = %d\n", i, f1_lines[i], x);
////        }
//    }
//    int len = strlen(f1_lines[4]);
//    int check = f1_lines[4][len - 1] == ':';
//    printf("%d\n", check);

//    FileLines *f_l = two_pass(f1);
//    for (int i = 0; i < f_l->length; i++) {
//        printf("%d: %s\n", i, f_l->lines[i]);
//    }

//    FirstPass *first_pass = split_lines(f1);
//    for (int i = 0; i < first_pass->length; i++) {
//        printf("%s\n", first_pass->lines[i]);
//    }


    return EXIT_SUCCESS;
}
