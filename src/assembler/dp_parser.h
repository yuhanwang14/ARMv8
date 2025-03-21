#include "util.h"

// parse madd/msub,
// arguments should be an array with 4 elements
uint32_t parse_multiply(char *opcode, char **arguments);

// parse add(s)/sub(s)/and(s)/bic(s)/eor/eon/orr/orn,
// arguments should be an array with 3 or 4 arguments
uint32_t parse_2op_with_dest(char *opcode, char **arguments);

// parse movn/movk/movz,
// arguments should be an array with 3 arguments
uint32_t parse_wide_move(char *opcode, char **arguments);