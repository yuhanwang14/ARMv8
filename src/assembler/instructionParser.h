#include <string.h>
#include <stdbool.h>
#include <stdio.h>

extern int instructionParser(char *instruction, int sizeIns);
// takes a single instruction and parse it into corresponding binary code

int multParser(char *instruction, int sizeIns);

int twoOprParser(char *instruction, int sizeIns);

int uniOprParser(char *instruction, int sizeIns);

int twoOprNoDesParser(char *instruction, int sizeIns);

int flatten(char *targets, int sizeArray);

char *fetchRegister(char *registerName);

bool isImmediate(char *target);

char *immediate12Parser(char *immediateValue, char *shift);

char *immediate6Parser(char *immediateValue);