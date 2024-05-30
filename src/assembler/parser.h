#include "dp_parser.h"
#include "dt_parser.h"
#include "branch_parser.h"
#include "parse_dir.h"

// takes a single instruction and parse it into corresponding binary code
uint32_t parse_instruction(char *instruction, uint32_t currentLoc);
