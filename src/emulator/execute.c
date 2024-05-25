#include "execute.h"

void execute(Register *reg, Instr *instr) {
    switch (instr->type) {
    DP_IMMEDIATE:
        // do something
        break;
    // TODO: handle other cases
    default:
        break;
    }
}
