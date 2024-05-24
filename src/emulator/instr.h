#include <stdint.h>

#ifndef _INSTR_H
#define _INSTR_H

typedef enum {
    DP_IMMEDIATE,
    DP_REGISTER,
    SINGLE_DATA_TRANSFER,
    LOAD_LITERAL,
    BRANCH
} InstrType;

typedef struct {
    uint32_t sf;
    uint32_t opc;
    uint32_t opi;
    uint32_t operand;
    uint32_t rd;
} DpImmed;

// A GADT Instr(uction) that can be a
//      - DP Immediate
//      - DP Register
//      - Single Data Transfer
//      - Load Literal
//      - Branch
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
