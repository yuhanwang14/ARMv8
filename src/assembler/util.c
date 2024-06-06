#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// consts for parse_shift
static const uint8_t LSL_SHIFT_CODE = 0;
static const uint8_t LSR_SHIFT_CODE = 1;
static const uint8_t ASR_SHIFT_CODE = 2;
static const uint8_t ROR_SHIFT_CODE = 3;

// consts for parse_register
static const uint8_t ADR_ZR = 31;

// consts for parse_imm16
static const uint32_t HW = 16;

FILE *safe_open(char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (f == NULL) {
        fprintf(stderr, "Failed to open %s with mode %s", path, mode);
        exit(EXIT_FAILURE);
    }
    return f;
}

uint8_t *parse_shift(char *shiftArg, char *shiftVal) {
    // code for the shift and the shifted value
    // returns a malloced array, should be freed after use
    uint8_t *result = malloc(2 * sizeof(uint8_t));
    result[0] = 0;
    result[1] = 0;
    if (shiftArg == NULL)
        // no shift was passed in at all
        return result;
    if (STR_EQ(shiftArg, "lsl")) {
        // result[0](shift code) for lsl is 0
        result[0] = LSL_SHIFT_CODE;
    } else if (STR_EQ(shiftArg, "lsr")) {
        // shift code for lsr is 0b01
        result[0] = LSR_SHIFT_CODE;
    } else if (STR_EQ(shiftArg, "asr")) {
        // shift code for asr is 0b10
        result[0] = ASR_SHIFT_CODE;
    } else if (STR_EQ(shiftArg, "ror")) {
        // shift code for asr is 0b11
        result[0] = ROR_SHIFT_CODE;
    } else {
        fprintf(stderr, "failed to parse shift for '%s'\n", shiftArg);
        exit(EXIT_FAILURE);
    }
    result[1] = parse_imm6(shiftVal);
    return result;
}

uint8_t parse_register(char *registerName) {
    if (registerName[0] != 'w' && registerName[0] != 'x') {
        // all register names start with 'w' or 'x'
        fprintf(stderr, "failed to parse register name '%s'\n", registerName);
        exit(EXIT_FAILURE);
    }
    if (STR_EQ(registerName + 1, "zr"))
        // handles zero registers, xzr/wzr
        return ADR_ZR;
    return strtol(registerName + 1, NULL, 0);
}

bool is_literal(char *target) { return (*target == '#'); }

uint32_t parse_imm16(char *literal, char *shiftCom, char *shiftVal) {
    if (shiftCom == NULL) {
        // if no shift is provided, return the value of literal
        return strtol(literal + 1, NULL, 0);
    }
    uint8_t *parsedShift = parse_shift(shiftCom, shiftVal);

    uint32_t result = parsedShift[1] / HW;
    // appends hw value
    bit_append(&result, strtol(literal + 1, NULL, 0), HW);
    free(parsedShift);
    return result;
}

uint16_t parse_imm12(char *literal, char *shiftCom, char *shiftVal) {
    if (shiftCom == NULL) {
        return strtol(literal + 1, NULL, 0);
    }
    uint8_t *parsedShift = parse_shift(shiftCom, shiftVal);
    if (parsedShift[0] != LSL_SHIFT_CODE) {
        // the shift code must be lsl
        fprintf(stderr, "failed to parse imm12 with shift %s", shiftCom);
        exit(EXIT_FAILURE);
    }
    if (parsedShift[1] == 0) {
        // if shift value is zero, the highest bit is 0
        free(parsedShift);
        return strtol(literal + 1, NULL, 0);
    }
    // if shift value is 12, the highest bit is set to 1
    free(parsedShift);
    return (strtol(literal + 1, NULL, 0) + (1 << 12));
}

uint8_t parse_imm6(char *literal) { return strtol(literal + 1, NULL, 0); }

void bit_append(uint32_t *target, int32_t appended, uint32_t length) {
    if (appended < 0) {
        // sign extension for negative append values
        *target = ((*target) << length) + appended + (1 << length);
        return;
    }
    *target = ((*target) << length) + appended;
    return;
}

bool is_shift(char *argument) {
    if (argument == NULL) {
        return false;
    }
    return (STR_EQ(argument, "lsl") || STR_EQ(argument, "lsr") || STR_EQ(argument, "asr") ||
            STR_EQ(argument, "ror"));
}

void insert_str(char **targetArray, int32_t arraySize, char *element, int8_t index) {
    for (int i = arraySize - 1; i >= index; i--) {
        targetArray[i + 1] = targetArray[i];
    }
    targetArray[index] = element;
}

int32_t parse_imm_general(char *strImm) { return strtol(strImm + 1, NULL, 0); }
