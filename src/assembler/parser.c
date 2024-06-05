#include "parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int8_t resolve_alias(char **opcode, char **arguments, int8_t numArg, char **buffer) {
    // this checks if any alias occur and change the opcode and arguments if so
    // buffer points to an allocated memory after running which must be freed after use
    // buffer must be initiallized with a value, i.e. not NULL
    // the type of buffer is a pointer to a string and is intended to be so, DO NOT CHANGE
    char *zeroReg = malloc(4*sizeof(char));
    *buffer = zeroReg;
    if (arguments[0][0] == 'x') {
        strcpy(zeroReg, "xzr");
    } else {
        strcpy(zeroReg, "wzr");
    }
    if (STR_EQ(*opcode, "cmp")) {
        *opcode = "subs";
        insert_str(arguments, numArg, zeroReg, 0);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "cmn")) {
        *opcode = "adds";
        insert_str(arguments, numArg, zeroReg, 0);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "neg")) {
        *opcode = "sub";
        insert_str(arguments, numArg, zeroReg, 1);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "negs")) {
        *opcode = "subs";
        insert_str(arguments, numArg, zeroReg, 1);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "tst")) {
        *opcode = "ands";
        insert_str(arguments, numArg, zeroReg, 0);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "mvn")) {
        *opcode = "orn";
        insert_str(arguments, numArg, zeroReg, 1);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "mov")) {
        *opcode = "orr";
        insert_str(arguments, numArg, zeroReg, 1);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "mul")) {
        *opcode = "madd";
        insert_str(arguments, numArg, zeroReg, 3);
        return numArg + 1;
    } else if (STR_EQ(*opcode, "mneg")) {
        *opcode = "msub";
        insert_str(arguments, numArg, zeroReg, 3);
        return numArg + 1;
    }
    return numArg;
}

uint32_t parse_instruction(char *instruction, uint32_t currentLoc) {
    char *opcode = strtok(instruction, " ,");
    if (STR_EQ(opcode, "ldr") || STR_EQ(opcode, "str"))
        return parse_sdt(opcode, strtok(NULL, ""), currentLoc); // special handler for DTIs
    char *nextArg = strtok(NULL, " ,");
    char *arguments[5] = {NULL, NULL, NULL, NULL, NULL};
    int numArg = 0;

    while (nextArg != NULL && numArg < 5) {
        // appends all arguments from the instruction
        arguments[numArg] = nextArg;
        numArg++;
        nextArg = strtok(NULL, " ,");
    }

    // buffer holds the pointer that must be freed after use(from resolve_alias)
    char **buffer = malloc(sizeof(char **));
    numArg = resolve_alias(&opcode, arguments, numArg, buffer);
    printf("instruction treated as '%s %s, %s, %s,%s, %s'\n", opcode, arguments[0], arguments[1],
           arguments[2], arguments[3], arguments[4]);
    uint32_t result;

    switch (numArg) {
    case 5:
        // 2 op with destination and a shift in operand
        // e.g. add x0, x0, #0, lsl #0
        result = parse_2op_with_dest(opcode, arguments);
        break;
    case 4:
        if (is_shift(arguments[2])) {
            // wide move (last two arguments assembles a shift)
            // e.g.movn x0, #0, lsl #0
            result = parse_wide_move(opcode, arguments);
        } else {
            // multiply operations(no shift applicable)
            // e.g. madd x0, x1, x2, x3
            result = parse_multiply(opcode, arguments);
        }
        break;
    case 3:
        // 2 op with destination, no shift in operand
        // e.g. add x0, x0, x0
        result = parse_2op_with_dest(opcode, arguments);
        break;
    case 2:
        // wide move without shift
        // e.g. movn x0, #2
        result = parse_wide_move(opcode, arguments);
        break;
    case 1:
        if (*opcode == '.') {
            // directive, one immediate value follows
            // e.g. .int #1234
            result = parse_dir(opcode, arguments);
        } else {
            // branching operation
            // e.g. b.eq #0
            result = parse_branch(opcode, arguments, currentLoc);
        }
        break;
    default:
        fprintf(stderr, "failed to parse\n'%s'\ndoes not have correct number of arguments",
                instruction);
        free(*buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }
    free(*buffer);
    free(buffer);
    return result;
}
