#include "instr.h"
#include <stdint.h>

// decode a 4-byte binary instruction into an Instr, a custom structure that represents any instruction
void decode(uint32_t code, Instr *instr);
