#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

extern int instructionParser(char *instruction);
// takes a single instruction and parse it into corresponding binary code

int multParser(char *instruction);

int twoOprParser(char *instruction);

int uniOprParser(char *instruction);

int twoOprNoDesParser(char *instruction);

int8_t registerParser(char *registerName);

bool isImmediate(char *target);
//checks whether an argument is an instance of immediate

int16_t immediate12Parser(char *immediateValue, char *shift);
// Parse 12 bit immediate values with possible shift, returns a 13-bit int

int8_t immediate6Parser(char *immediateValue);
// Parse 6-bit immediate value

uint8_t *shiftParser(char *shiftArg);
// Parse a shift argument
// returns an Array of size 2, first elem is the code of shift(2-bit), second elem is the shift value
// returns {0,0} if passed in NULL
