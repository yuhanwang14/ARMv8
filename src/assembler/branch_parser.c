#include "branch_parser.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint32_t PARSE_REG_START = 3508160;
static const uint8_t PARSE_UNCOND_START = 5;
static const uint8_t PARSE_COND_START = 84;
static const uint8_t COND_CODE_EQ = 0;
static const uint8_t COND_CODE_NE = 1;
static const uint8_t COND_CODE_GE = 10;
static const uint8_t COND_CODE_LT = 11;
static const uint8_t COND_CODE_GT = 12;
static const uint8_t COND_CODE_LE = 13;
static const uint8_t COND_CODE_AL = 14;

static uint32_t parse_uncond(char *opcode, char **arguments, int32_t currentLoc) {
    uint32_t result = PARSE_UNCOND_START;                                  // pos 26-31
    bit_append(&result, parse_imm_general(arguments[0]) - currentLoc, 26); // simm 26, pos 0 - 25
    // offset calculated here
    return result;
}

static uint32_t parse_with_reg(char *opcode, char **arguments, int32_t currentLoc) {
    uint32_t result = PARSE_REG_START;                                    // pos 10 - 31
    bit_append(&result, parse_register(arguments[0]), REGISTER_ADR_SIZE); // xn, pos 5 - 9
    bit_append(&result, 0, 5);                                            // pos 0-4
    return result;
}

static uint32_t parse_cond(char *opcode, char **arguments, int32_t currentLoc) {
    uint32_t result = PARSE_COND_START;                                    // pos 24-31
    bit_append(&result, parse_imm_general(arguments[0]) - currentLoc, 19); // simm19, pos 5 - 23
    bit_append(&result, 0, 1);                                             // pos 4
    if (strcmp(opcode, "b.eq") == 0) {                                     // cond, pos 0 - 4
        bit_append(&result, COND_CODE_EQ, 4);
    } else if (strcmp(opcode, "b.ne") == 0) {
        bit_append(&result, COND_CODE_NE, 4);
    } else if (strcmp(opcode, "b.ge") == 0) {
        bit_append(&result, COND_CODE_GE, 4);
    } else if (strcmp(opcode, "b.lt") == 0) {
        bit_append(&result, COND_CODE_LT, 4);
    } else if (strcmp(opcode, "b.gt") == 0) {
        bit_append(&result, COND_CODE_GT, 4);
    } else if (strcmp(opcode, "b.le") == 0) {
        bit_append(&result, COND_CODE_LE, 4);
    } else if (strcmp(opcode, "b.al") == 0) {
        bit_append(&result, COND_CODE_AL, 4);
    } else {
        fprintf(stderr, "failed to parse the condition of '%s'\n", opcode);
        exit(EXIT_FAILURE);
    }
    return result;
}

uint32_t parse_branch(char *opcode, char **arguments, uint32_t currentLoc) {
    if (*opcode != 'b') {
        fprintf(stderr, "failed to parse '%s' as branching instruction\n", opcode);
        exit(EXIT_FAILURE);
    }
    switch (strlen(opcode)) {
    case 1:
        // opcode is 'b'
        return parse_uncond(opcode, arguments, currentLoc);
    case 2:
        // opcode is 'br'
        return parse_with_reg(opcode, arguments, currentLoc);
    case 4:
        // opcode is "b.cond"
        return parse_cond(opcode, arguments, currentLoc);
    default:
        // unidentified opcode
        fprintf(stderr, "failed to parse '%s' as branching instruction\n", opcode);
        exit(EXIT_FAILURE);
    }
}
