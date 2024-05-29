#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef _UTIL_H
#define _UTIL_H
#define REGISTER_ADR_SIZE 5

uint8_t parse_register(char *registerName);
// takes a name of register, returns its address

bool is_literal(char *target);
//checks whether an argument is an instance of immediate

uint16_t parse_imm12(char *immediateValue, char *shift);
// Parse 12 bit immediate values with possible shift, returns a 13-bit int

uint32_t parse_imm16(char *immediateValue, char *shiftCom,char *shiftVal);
// Parse 16 bit immediate values with possible shift, returns an 18-bit int

uint8_t parse_imm6(char *immediateValue);
// Parse 6-bit immediate value

uint8_t *parse_shift(char *shiftArg,char *shiftVal);
// Parse a shift argument
// returns an Array of size 2, first elem is the code of shift(2-bit), second elem is the shift value
// returns {0,0} if passed in NULL

void appendBits(uint32_t *target,uint32_t appended,uint32_t length);
 // takes in a pointer to an int, appends the second parameter behind it.

bool is_shift(char *argument);
// checks if the argument is a shift operation

void insertChar(char **targetArray,int32_t arraySize,char *element, int8_t index);
 // takes an array of char with at least one free space, inserts the element at the required index

 int32_t safe_string_to_int(char *intStr);
 // identifies between decimal and hex, then perform the correct atoi operation
#endif