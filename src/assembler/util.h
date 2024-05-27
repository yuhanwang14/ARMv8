#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef _UTIL_H
#define _UTIL_H
const uint8_t REGISTER_ADR_SIZE = 5;

uint8_t parse_register(char *registerName);

bool is_literal(char *target);
//checks whether an argument is an instance of immediate

uint16_t parse_imm12(char *immediateValue, char *shift);
// Parse 12 bit immediate values with possible shift, returns a 13-bit int

uint8_t parse_imm6(char *immediateValue);
// Parse 6-bit immediate value

uint8_t *parse_shift(char *shiftArg);
// Parse a shift argument
// returns an Array of size 2, first elem is the code of shift(2-bit), second elem is the shift value
// returns {0,0} if passed in NULL

void appendBits(uint32_t *target,uint32_t appended,uint32_t length);

#endif