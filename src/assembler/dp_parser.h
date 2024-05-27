#include "util.h"

uint32_t parse_multiply(char *instruction);
// parse madd/msub

uint32_t parse_two_operands_with_des(char *instruction);
// parse add(s)/sub(s)/and(s)/bic(s)/eor/eon/orr/orn

uint32_t parse_wide_move(char *instruction);
// parse movn/movk/movz

uint32_t parse_two_operands_no_des(char *instruction);