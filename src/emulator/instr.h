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
typedef enum { DPI_ARITHMETIC_T, WIDE_MOVE_T } DpImmedType;
typedef enum { ADD = 0, ADDS = 1, SUB = 2, SUBS = 3 } DpArithmeticType;
typedef enum { MOVN = 0, MOVZ = 2, MOVK = 3 } DpWideMoveType;

typedef struct {
    DpImmedType type;
    union {
        struct {
            bool sf;
            bool sh;
            DpArithmeticType opc;
            uint32_t imm12;
            uint32_t rn;
            uint32_t rd;
        } arithmetic;
        struct {
            bool sf;
            uint32_t hw;
            uint32_t opc;
            uint32_t imm16;
            uint32_t rd;
        } wide_move;
    };
} DpImmed;

// A DP Register instruction is one of:
//      - arithmetic
//      - bit-logic
//      - multiply
typedef enum { DPR_ARITHMETIC_T, BIT_LOGIC_T, MULTIPLY_T } DpRegisterType;
typedef enum { A_LSL_T = 0, A_LSR_T = 1, A_ASR_T = 2 } AritShiftType;
typedef enum {
    L_LSL_T = 0,
    L_LSR_T = 1,
    L_ASR_T = 2,
    L_ROR_T = 3
} LogcShiftType;
typedef enum {
    AND = 0,
    OR = 1,
    EO = 2,
    ANDC = 3,
} BitInstrType;
typedef DpArithmeticType DrArithmeticType;

typedef struct {
    DpRegisterType type;
    union {
        struct {
            AritShiftType stype;
            DrArithmeticType atype;
            uint32_t rd;
            uint32_t rn;
            uint32_t rm;
            bool sf;
            uint32_t shift;
        } arithmetic;
        struct {
            LogcShiftType stype;
            BitInstrType btype;
            uint32_t rd;
            uint32_t rn;
            uint32_t rm;
            bool sf;
            uint32_t shift;
            bool N;
        } logical;
        struct {
            uint32_t rd;
            uint32_t rn;
            uint32_t rm;
            bool sf;
            bool x;
            uint32_t ra;
        } multiply;
    };
} DpRegister;

// A GADT Instr(uction) that is one of:
//      - DP Immediate
//      - DP Register
//      - Single Data Transfer
//      - Load Literal
//      - Branch
typedef enum {
    DP_IMMEDIATE_T,
    DP_REGISTER_T,
    SINGLE_DATA_TRANSFER_T,
    LOAD_LITERAL_T,
    BRANCH_T
} InstrType;

typedef struct {
    InstrType type;
    union {
        DpImmed dp_immed;
        DpRegister dp_reg;
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

#endif
