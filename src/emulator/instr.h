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
            bool sf;                // 32/64 mode flag
            bool sh;                // whether imm12 should be moved left by 12 bits
            DpArithmeticType atype; // operation type
            uint32_t imm12;         // Op2
            uint32_t rn;            // source register index
            uint32_t rd;            // destination register index
        } arithmetic;
        struct {
            bool sf;              // 32/64 mode flag
            uint32_t hw;          // imm16 should be left shifted by hw * 16 bits
            DpWideMoveType mtype; // operation type
            uint32_t imm16;       // operand
            uint32_t rd;          // destination register index
        } wide_move;
    };
} DpImmed;

// A DP Register instruction is one of:
//      - arithmetic
//      - bit-logic
//      - multiply
typedef enum { DPR_ARITHMETIC_T, BIT_LOGIC_T, MULTIPLY_T } DpRegisterType;
typedef enum { A_LSL_T = 0, A_LSR_T = 1, A_ASR_T = 2 } AritShiftType;
typedef enum { L_LSL_T = 0, L_LSR_T = 1, L_ASR_T = 2, L_ROR_T = 3 } LogcShiftType;
typedef enum { MADD = 0, MSUB = 1 } MultType;
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
            AritShiftType stype;    // right operand preprocessing: how should it be shifted
            DrArithmeticType atype; // operation type
            uint32_t rd;            // destination register index
            uint32_t rn;            // left operand
            uint32_t rm;            // right operand
            bool sf;                // 32/64 mode flag
            uint32_t shift;         // right operand preprocessing: how many bits should it be shifted
        } arithmetic;
        struct {
            LogcShiftType stype; // preprocessing operation type
            BitInstrType btype;  // operation type
            uint32_t rd;         // destination register index
            uint32_t rn;         // left operand
            uint32_t rm;         // right operand
            bool sf;             // 32/64 mode flag
            uint32_t shift;      // right operand preprocessing: how many bits should it be shifted
            bool N;              // right operand preprocessing: bit negated after shift?
        } logical;
        struct {
            uint32_t rd; // destination register index
            uint32_t rn; // Rn
            uint32_t rm; // Rm
            bool sf;     // 32/64 mode flag
            MultType x;  // operation type
            uint32_t ra; // Ra
        } multiply;
    };
} DpRegister;

// [yw8123]:

// A Single Data Transfer consists of
//      - Load
//      - Store

// The Transfer address is depended on the offset,
// there are three kinds of offsets:
//		- Register offset
// 		- Pre/Post-indexed offset
// 		- Unsigned offset

typedef enum { REGISTER, PRE_POST_INDEX, UNSIGN } SDTransType;
typedef enum { POST_INDEX = 0, PRE_INDEXED = 1 } IndexType;

typedef struct {
    SDTransType type;
    union {
        struct {
            uint64_t xm; // Another X-register
            bool sf;     // The size of the load, 0 is 32-bit, 1 is 64-bit
            bool L;      // The type of data transfer, 0 is store, 1 is load.
            uint64_t xn; // The base X-register
            uint64_t rt; // The target register
        } reg;
        struct {
            IndexType itype; // pre-indexed / post-indexed.
            uint32_t simm9;
            bool sf; // 32/64 mode flag
            bool L;
            uint64_t xn;
            uint64_t rt;
        } pre_post_index;
        struct {
            uint32_t imm12;
            bool sf; // 32/64 mode flag
            bool L;
            uint64_t xn;
            uint64_t rt;
        } unsign;
    };
} SdTrans;

// Load Literal is similar to data transfer.

typedef struct {
    bool sf;
    uint32_t simm19;
    uint64_t rt;
} LoadLiteral;

// A GADT Instr(uction) that is one of:
//      - DP Immediate
//      - DP Register
//      - Single Data Transfer
//      - Load Literal
//      - Branch
typedef enum { DP_IMMEDIATE_T, DP_REGISTER_T, SINGLE_DATA_TRANSFER_T, LOAD_LITERAL_T, BRANCH_T } InstrType;

typedef struct {
    InstrType type;
    union {
        DpImmed dp_immed;
        DpRegister dp_reg;
        SdTrans sing_data_transfer;
        LoadLiteral load_literal;
        struct {
            // TODO
        } branch;
    };
} Instr;

#endif
