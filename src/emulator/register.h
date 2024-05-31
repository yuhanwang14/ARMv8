#include <stdbool.h>
#include <stdint.h>

#ifndef _REGISTER_H
#define _REGISTER_H

// hence the 2Mib memory contains this many 4-bytes words
#define WORD_COUNT (1 << 19)
#define BYTE_COUNT (1 << 21)
// number of registers
#define REG_COUNT 31

typedef struct {
    bool N; // Negative condition flag
    bool Z; // Zero condition flag
    bool C; // Carry condition flag
    bool V; // oVerflow condition flag
} PState;

// represents the register machine we are modifying
typedef struct {
    uint64_t g_reg[REG_COUNT]; // general purpose register
    uint64_t ZR;               // zero register
    uint64_t PC;               // program counter
    PState *PSTATE;            // program state
    uint8_t ram[BYTE_COUNT];   // virtual memory
} Register;

// Initialilze the registers according to the spec
// Returns a register struct with Z flag set and PC at zero
Register *reg_init(void);

// frees a register struct
void reg_free(Register *reg);

#endif
