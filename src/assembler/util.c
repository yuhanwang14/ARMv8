#include "util.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t *parse_shift(char *shiftArg, char *shiftVal) {
    uint8_t *result = malloc(2 * sizeof(uint8_t));
    result[0] = 0;
    result[1] = 0;
    if (shiftArg == NULL)
        return result;
    if (strcmp(shiftArg, "lsl") == 0) {
    } else if (strcmp(shiftArg, "lsr") == 0) {
        result[0] = 1;
    } else if (strcmp(shiftArg, "asr") == 0) {
        result[0] = 2;
    } else if (strcmp(shiftArg, "ror") == 0) {
        result[0] = 3;
    } else {
        fprintf(stderr, "failed to parse shift for '%s'\n", shiftArg);
        exit(EXIT_FAILURE);
    }
    printf("parse pos 1\n%s\n", shiftArg);
    result[1] = parse_imm6(shiftVal);
    return result;
}

uint8_t parse_register(char *registerName) {
    if (registerName[0] != 'w' && registerName[0] != 'x') {
        fprintf(stderr, "failed to parse register name '%s'\n", registerName);
        exit(EXIT_FAILURE);
    }
    if (strcmp(registerName + 1, "zr") == 0)
        return 31;
    return strtol(registerName + 1, NULL, 0);
}

bool is_literal(char *target) { return (*target == '#'); }

uint32_t parse_imm16(char *literal, char *shiftCom, char *shiftVal) {
    if (shiftCom == NULL)
        return strtol(literal + 1, NULL, 0);
    uint8_t *parsedShift = parse_shift(shiftCom, shiftVal);
    uint32_t result = parsedShift[1] / 4;
    bit_append(&result, strtol(literal + 1, NULL, 16), 16);
    free(parsedShift);
    return result;
}

uint16_t parse_imm12(char *literal, char *shiftCom, char *shiftVal) {
    if (shiftCom == NULL) {
        return strtol(literal + 1, NULL, 0);
    }
    uint8_t *parsedShift = parse_shift(shiftCom, shiftVal);
    if (parsedShift[0] != 0){
        fprintf(stderr, "failed to parse imm12 with shift %s", shiftCom);
        exit(EXIT_FAILURE);
    }
    if (parsedShift[1] == 0){
        return strtol(literal + 1, NULL, 0);
    }
    return (strtol(literal + 1, NULL, 0) + (1 << 12));
}

uint8_t parse_imm6(char *literal) { return strtol(literal + 1, NULL, 0); }

void bit_append(uint32_t *target, int32_t appended, uint32_t length) {
    if (appended < 0){
        *target = ((*target) << length) + appended + (1 << length);
        return;
    };
    *target = ((*target) << length) + appended;
    return;
}

bool is_shift(char *argument) {
    if (argument == NULL)
        return false;
    return (strcmp(argument, "lsl") == 0 || strcmp(argument, "lsr") == 0 || strcmp(argument, "asr") == 0 ||
            strcmp(argument, "ror") == 0);
}

void insert_str(char **targetArray, int32_t arraySize, char *element, int8_t index) {
    for (int i = arraySize - 1; i >= index; i--) {
        targetArray[i + 1] = targetArray[i];
    }
    targetArray[index] = element;
}

int32_t parse_imm_general(char *strImm) { return strtol(strImm + 1, NULL, 0); }
