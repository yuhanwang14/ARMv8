#include <stdbool.h>
#include <stdint.h>

#ifndef _INSTR_H
#define _INSTR_H

// [ky723]:
// Every instruction is a GADT that's separated into categories, and every
// category itself is a GADT.
// Using this design allows for finer-tuned type checking and less unclear
// branching, though presumably results in higher memory use, but this is so
// cool I have to try it :)

// A DP Immediate instruction is one of:
//      - arithmetic
//      - wide move
typedef enum { ARITHMETIC, WIDE_MOVE } DpInstrType;

typedef struct {
    DpInstrType type;
    bool sf;
    uint32_t rd;
    union {
        struct {
            bool sh;
            uint32_t imm12;
            uint32_t rn;
        } arithmetic;
        struct {
            uint32_t hw;
            uint32_t imm16;
        } wide_move;
    };
} DpImmed;

// A GADT Instr(uction) that is one of:
//      - DP Immediate
//      - DP Register
//      - Single Data Transfer
//      - Load Literal
//      - Branch
typedef enum {
    DP_IMMEDIATE,
    DP_REGISTER,
    SINGLE_DATA_TRANSFER,
    LOAD_LITERAL,
    BRANCH
} InstrType;

typedef struct {
    InstrType type;
    union {
        DpImmed *dp_immed;
        struct {
            // TODO
        } dp_reg;
        struct {
            // TODO
        } sing_data_transfer;
        struct {
            // TODO
        } load_literal;
        struct {
            // TODO
        } branch;
    };
} Instr;

// frees a Instr struct
void instr_free(Instr *instr);

#endif
