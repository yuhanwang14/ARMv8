#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


#ifndef _REGISTER_H
#define _REGISTER_H

#define MEMORY_SIZE (size_t)(1 << 14)
// I have change 2MB to (1<<20) and
// then i think this is the right answer

typedef struct {
    bool N; // Negative condition flag
    bool Z; // Zero condition flag
    bool C; // Carry condition flag
    bool V; // oVerflow condition flag
} PState;

typedef struct {
    uint64_t g_reg[31];        // general purpose register
    uint64_t ZR;               // zero register
    uint64_t PC;               // program counter
    PState *PSTATE;            // program state
    uint32_t ram[MEMORY_SIZE]; // virtual memory
} Register;

// Initialilze the registers according to the spec
// Returns a register struct with Z flag set and PC at zero
Register *reg_init(void);

// frees a register struct
void reg_free(Register *reg);

#endif
