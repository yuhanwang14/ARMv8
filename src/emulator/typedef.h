#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MEMORY_SIZE 1 << 20 // how many bytes are 2MB

typedef struct {
    bool N; // this is the negative condition flag
    bool Z; // this is the zero condition flag
    bool C; // this is the carry condition flag
    bool V; // this is the overflow condition flag
} PState;

typedef struct {
    uint64_t g_reg[31];        // this is the general purpose register
    uint64_t ZR;               // this is the zero register
    uint64_t PC;               // this is the program counter
    PState PSTATE;             // this is the program state
    uint32_t ram[MEMORY_SIZE]; // this is our memory
} Register;
