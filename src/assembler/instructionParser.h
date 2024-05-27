#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t parse_instruction(char *instruction);
// takes a single instruction and parse it into corresponding binary code

uint32_t parse_multiply(char *instruction);

uint32_t parse_two_operands_with_des(char *instruction);

uint32_t parse_uni_operand(char *instruction);

uint32_t parse_two_operands_no_des(char *instruction);

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
