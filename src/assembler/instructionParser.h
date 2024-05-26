#include <string.h>

extern int *instructionParser(char *instruction, int sizeIns);
// takes a single instruction and parse it into corresponding binary code

int branchParser(char *instruction, int sizeIns);
// helper. Parses branch instructions into binary

int aritParser(char *instruction, int sizeIns);
// helper. Parses arithmetic instructions into binary

int cmpParser(char *instruction, int sizeIns);
// helper. Parses compare instructions into binary

int logicInsParser(char *instruction, int sizeIns);
// helper. Parses logic instructions into binary

int tstParser(char *instruction, int sizeIns);
// helper. Parses test bits instruction into binary using logicInsParser

int mvnTransParser(char *instruction, int sizeIns);
// helper. Parses mvn instruction into binary using logicInsParser

int movParser(char *instruction, int sizeIns);
// helper. Parses mov instruction into binary

int wMovParser(char *instruction, int sizeIns);
// helper. Parses wide move instructions into binary

int m4Parser(char *instruction, int sizeIns);
// helper. Parses madd and msub instructions into binary

int dataTransParser(char *instruction, int sizeIns);
// helper. Parses str and ldr instructions into binary
