#include "dt_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t SDT_LITERAL = 24;
static const uint8_t SDT_NOT_LITERAL_25_29 = 28;
static const uint8_t OFFSET_UNSIGNED_SIZE = 12;
static const uint8_t SDT_LITERAL_SIZE = 6;
static const uint8_t SIMM19_SIZE = 19;
static const uint8_t SIMM9_SIZE = 9;
static const uint8_t SDT_REG_10_15 = 26;
static const uint8_t LDR_L_FLAG = 1;
static const uint8_t STR_L_FLAG = 0;
static const uint8_t SDT_REG_L_FLAG_SIZE = 1;
static const uint8_t SDT_POST_I_FLAG = 0;
static const uint8_t SDT_PRE_I_FLAG = 1;
static const uint8_t SDT_INDEXED_I_FLAG_SIZE = 1;
static const uint8_t SDT_CASE_SPEC_CODE_SIZE = 20;


static int32_t parse_simm19(char *absoluteAddress, uint32_t currentLoc) {
    int32_t offset = strtol(absoluteAddress + 1, NULL, 0) - currentLoc;
    // performs sign-extension for negative offsets
    return offset > 0 ? offset : offset + (1 << SIMM19_SIZE);
}

static uint32_t parse_dt_literal(char *rt, char *immediate, uint32_t currentLoc) {
    uint32_t result = 0;                                                   // pos 31
    bit_append(&result, GET_SF(rt), SF_SIZE);                              // sf, pos 30
    bit_append(&result, SDT_LITERAL, SDT_LITERAL_SIZE);                    // pos 24-29
    bit_append(&result, parse_simm19(immediate, currentLoc), SIMM19_SIZE); // simm 19, pos 5-23
    bit_append(&result, parse_register(rt), REGISTER_ADR_SIZE);            // rt, pos 0-4
    return result;
}

static uint32_t parse_indexed_or_reg(char *opcode, char **addressArg, int8_t numArg) {
    uint32_t result = 0; // pos 23, 24 are both 0
    if (STR_EQ(opcode, "str")) {
        bit_append(&result, STR_L_FLAG, SDT_REG_L_FLAG_SIZE); // 'L'bit, pos 22
    } else if (STR_EQ(opcode, "ldr")) {
        bit_append(&result, LDR_L_FLAG, SDT_REG_L_FLAG_SIZE);
    } else {
        fprintf(stderr, "failed to parse opcode '%s' as a data transfer", opcode);
        exit(EXIT_FAILURE);
    }
    if (is_literal(addressArg[1])) {
        // pre/post indexed
        printf("dt_instruction parsed as indexed\n");
        bit_append(&result, 0, 1);                                // pos 21
        bit_append(&result, parse_imm_general(addressArg[1]), SIMM9_SIZE); // simm 9, pos 12-20
        if (numArg == 3) {
            // pre-indexed
            bit_append(&result, SDT_PRE_I_FLAG, SDT_INDEXED_I_FLAG_SIZE); // 'I', pos 11
        } else {
            // post-indexed
            bit_append(&result, SDT_POST_I_FLAG, SDT_INDEXED_I_FLAG_SIZE);
        }
        bit_append(&result, 1, 1); // pos 10
    } else {
        printf("dt_instruction parsed as register\n");
        bit_append(&result, 1, 1);                                             // pos 21
        bit_append(&result, parse_register(addressArg[1]), REGISTER_ADR_SIZE); // xm, pos 16-20
        bit_append(&result, SDT_REG_10_15, 6);                                            // pos 10-15
    }
    bit_append(&result, parse_register(addressArg[0]), REGISTER_ADR_SIZE); // xn, pos 5-9
    printf("%i\n", result);
    return result;
}

static uint32_t parse_unsigned(char *opcode, char **addressArg, int8_t numArg, bool is64) {
    uint32_t result = 2; // the highest digit is '1' since it is an unsigned offset
    if (STR_EQ(opcode, "str")) {
        bit_append(&result, STR_L_FLAG, SDT_REG_L_FLAG_SIZE); // 'L' bit, pos 22
    } else if (STR_EQ(opcode, "ldr")) {
        bit_append(&result, LDR_L_FLAG, SDT_REG_L_FLAG_SIZE);
    } else {
        fprintf(stderr, "failed to parse '%s' as a data transfer", opcode);
        exit(EXIT_FAILURE);
    }
    if (numArg == 1) {
        // no immediate supplied
        bit_append(&result, 0, OFFSET_UNSIGNED_SIZE); // offset, pos 10-21
    } else {
        if (is64) {
            bit_append(&result, parse_imm_general(addressArg[1]) / 8, OFFSET_UNSIGNED_SIZE);
        } else {
            bit_append(&result, parse_imm_general(addressArg[1]) / 4, OFFSET_UNSIGNED_SIZE);
        }
    }
    bit_append(&result, parse_register(addressArg[0]), REGISTER_ADR_SIZE); // xn, pos 5-9
    return result;
}

uint32_t parse_sdt(char *opcode, char *argument, uint32_t currentLoc) {
    char *rt = strtok(argument, " ,");
    char *rest = strtok(NULL, "");
    if (*rest == ' ') {
        rest++;
    }
    if (*rest == '#') {
        // the second argument is an immediate value, instruction parsed as dt_literal
        return parse_dt_literal(rt, rest, currentLoc);
    }
    while (*rest == ' ') {
        rest++; // removes all spaces at the front(if any)
    }
    rest = rest + 1;
    // if we reach here, it implies the original str starts with '[', remove the bracket
    bool postIndex = rest[3] == ']' ||
                     rest[2] == ']'; // this is used to distinguish Unsigned offset and post-index
    char *nextArg = strtok(rest, ",] ");
    char *addressArg[3];
    int8_t numArg = 0;
    while (nextArg != NULL && numArg < 3) {
        addressArg[numArg] = nextArg;
        numArg++;
        nextArg = strtok(NULL, ",] ");
    }
    uint32_t result = 1;                           // pos 31
    bit_append(&result, GET_SF(rt), SF_SIZE);            // sf, pos 30
    bit_append(&result, SDT_NOT_LITERAL_25_29, 5); // pos 25-29
    switch (numArg) {
    case 1:
        // Unsigned offset without an immediate offset
        // only one register name in brackets
        printf("dt_instruction parsed as unsigned offset\n");
        bit_append(&result, parse_unsigned(opcode, addressArg, numArg, *rt == 'x'), SDT_CASE_SPEC_CODE_SIZE); // pos 5-24
        break;
    case 2:
        if ((!postIndex) && is_literal(addressArg[1])) {
            // Unsigned offset with an immediate offset
            // the second argument in brackets is a literal, and is confirmed not a post-index
            printf("dt_instruction parsed as unsigned offset\n");
            bit_append(&result, parse_unsigned(opcode, addressArg, numArg, *rt == 'x'), SDT_CASE_SPEC_CODE_SIZE);
        } else {
            // other cases, can be addressed register or post indexed
            printf("dt_instruction parsed as index or register\n");
            bit_append(&result, parse_indexed_or_reg(opcode, addressArg, numArg), SDT_CASE_SPEC_CODE_SIZE);
        }
        break;
    case 3:
        // pre-indexed address, the last argument would be '!'
        printf("dt_instruction parsed as index or register\n");
        bit_append(&result, parse_indexed_or_reg(opcode, addressArg, numArg), SDT_CASE_SPEC_CODE_SIZE);
    }
    bit_append(&result, parse_register(rt), REGISTER_ADR_SIZE); // rt, pos 0 - 4
    return result;
}
