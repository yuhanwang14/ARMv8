#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef _UTIL_H
#define _UTIL_H

#define REGISTER_ADR_SIZE 5
// determine whether this instruction is 64 mode (register name starts with x)
#define GET_SF(rn) (*rn == 'x' ? 1 : 0)

FILE *safe_open(char *path, const char *mode);

// takes a name of register, returns its address
uint8_t parse_register(char *registerName);

// checks whether an argument is an immediate operand
bool is_literal(char *target);

// Parse 12 bit immediate values with possible shift, returns a 13-bit int
uint16_t parse_imm12(char *literal, char *shiftCom, char *shiftVal);

// Parse 16 bit immediate values with possible shift, returns an 18-bit int
uint32_t parse_imm16(char *immediateValue, char *shiftCom, char *shiftVal);

// Parse 6-bit immediate value
uint8_t parse_imm6(char *immediateValue);

// Parse a shift argument,
// returns an Array of size 2, first elem is the code of shift(2-bit), second elem is the shift
// value, returns {0,0} if passed in NULL Note: the returned array is created from malloc, should be
// freed after usage
uint8_t *parse_shift(char *shiftArg, char *shiftVal);

// takes in a pointer to an int, appends the second parameter behind it.
void bit_append(uint32_t *target, int32_t appended, uint32_t length);

// checks if the argument is a shift operation
bool is_shift(char *argument);

// takes an array of char with at least one free space, inserts the element at the required index
void insert_str(char **targetArray, int32_t arraySize, char *element, int8_t index);

// parses general immediate values to integers
int32_t parse_imm_general(char *strImm);

#endif
