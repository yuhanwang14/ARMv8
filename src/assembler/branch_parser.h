#include "util.h"

uint32_t branch_parser(char *opcode, char **arguments, int32_t currentLoc);
// parse all branching opcodes
// argument should be an array with one element