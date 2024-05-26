#include <string.h>

extern int *instructionParser(char *instruction, int sizeIns);
// takes a single instruction and parse it into corresponding binary code

int multParser(char *instruction, int sizeIns);

int twoOprParser(char *instruction, int sizeIns);

int uniOprParser(char *instruction, int sizeIns);

int twoOprNoDesParser(char *instruction, int sizeIns);
