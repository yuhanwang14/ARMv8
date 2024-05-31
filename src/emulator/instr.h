#include <stdbool.h>
#include <stdint.h>

#ifndef _INSTR_H
#define _INSTR_H

// [ky723]:
// Every instruction is a GADT that's separated into categories, and every
// category itself is a GADT.
// Using this design allows for finer-tuned type checking and less unclear
// branching, though presumably results in higher memory use. This is so
// cool I have to try it :)

// A DP Immediate instruction is one of:
//      - arithmetic
//      - wide move
typedef enum { DPI_ARITHMETIC_T, WIDE_MOVE_T } DpImmedType;
typedef enum { ADD = 0, ADDS = 1, SUB = 2, SUBS = 3 } ArithmeticType;
typedef enum { MOVN = 0, MOVZ = 2, MOVK = 3 } DpWideMoveType;

typedef struct {
    bool sf;              // 32/64 mode flag
    bool sh;              // whether imm12 should be moved left by 12 bits
    ArithmeticType atype; // operation type
    uint32_t imm12;       // Op2
    uint32_t rn;          // source register index
    uint32_t rd;          // destination register index
} DPIArithmetic;

typedef struct {
    bool sf;              // 32/64 mode flag
    uint32_t hw;          // imm16 should be left shifted by hw * 16 bits
    DpWideMoveType mtype; // operation type
    uint32_t imm16;       // operand
    uint32_t rd;          // destination register index
} WideMove;

// DP (Immediate)
typedef struct {
    DpImmedType type;
    union {
        DPIArithmetic arithmetic;
        WideMove wide_move;
    };
} DpImmed;

// A DP Register instruction is one of:
//      - arithmetic
//      - bit-logic
//      - multiply
typedef enum { DPR_ARITHMETIC_T, BIT_LOGIC_T, MULTIPLY_T } DpRegisterType;
typedef enum { A_LSL_T = 0, A_LSR_T = 1, A_ASR_T = 2 } AritShiftType;
typedef enum { L_LSL_T = 0, L_LSR_T = 1, L_ASR_T = 2, L_ROR_T = 3 } LogcShiftType;
typedef enum {
    AND = 0,
    OR = 1,
    EO = 2,
    ANDC = 3,
} BitInstrType;

typedef struct {
    AritShiftType stype;  // right operand preprocessing: how should it be shifted
    ArithmeticType atype; // operation type
    uint32_t rd;          // destination register index
    uint32_t rn;          // left operand
    uint32_t rm;          // right operand
    bool sf;              // 32/64 mode flag
    uint32_t shift;       // right operand preprocessing: how many bits should it be shifted
} DPRArithmetic;

typedef struct {
    LogcShiftType stype; // preprocessing operation type
    BitInstrType btype;  // operation type
    uint32_t rd;         // destination register index
    uint32_t rn;         // left operand
    uint32_t rm;         // right operand
    bool sf;             // 32/64 mode flag
    uint32_t shift;      // right operand preprocessing: how many bits should it be shifted
    bool N;              // right operand preprocessing: bit negated after shift?
} Logical;

typedef struct {
    uint32_t rd; // destination register index
    uint32_t rn; // Rn
    uint32_t rm; // Rm
    bool sf;     // 32/64 mode flag
    bool x;      // operation type
    uint32_t ra; // Ra
} Multiply;

// DP (Register)
typedef struct {
    DpRegisterType type;
    union {
        DPRArithmetic arithmetic;
        Logical logical;
        Multiply multiply;
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

typedef enum { SD_REGISTER_T, PRE_POST_INDEX_T, UNSIGN_T } SDTransType;
typedef enum { POST_INDEX, PRE_INDEX } IndexType;

typedef struct {
    uint64_t xm; // Another X-register
    bool sf;     // The size of the load, 0 is 32-bit, 1 is 64-bit
    bool L;      // The type of data transfer, 0 is store, 1 is load.
    uint64_t xn; // The base X-register
    uint64_t rt; // The target register
} SDRegister;

typedef struct {
    IndexType itype; // pre-indexed / post-indexed.
    int32_t simm9;   // signed
    bool sf;         // 32/64 mode flag
    bool L;
    uint64_t xn;
    uint64_t rt;
} PrePostIndex;

typedef struct {
    uint32_t imm12;
    bool sf; // 32/64 mode flag
    bool L;
    uint64_t xn;
    uint64_t rt;
} Unsigned;

// Single data transfer
typedef struct {
    SDTransType type;
    union {
        SDRegister reg;
        PrePostIndex pre_post_index;
        Unsigned usigned;
    };
} SdTrans;

// Load Literal is similar to data transfer.

// Load literal
typedef struct {
    bool sf;
    int32_t simm19;
    uint64_t rt;
} LoadLiteral;

// A Branch is one of:
//      - Unconditional
//      - Register
//      - Conditional
typedef enum { UNCONDITIONAL_T = 0x5, BR_REGISTER_T = 0x35, CONDITIONAL_T = 0x15 } BranchType;
typedef enum { EQ = 0, NE = 1, GE = 10, LT = 11, GT = 12, LE = 13, AL = 14 } CondType;

typedef struct {
    uint32_t offset; // the offset to be applied to PC
} Unconditional;

typedef struct {
    uint32_t xn; // the register containing the address to jump to
} BranchReg;

typedef struct {
    int64_t offset; // the offset to be applied to PC
    CondType cond;  // conditions that PSTATE must meet to apply the jump
} Conditional;

// Branch
typedef struct {
    BranchType type;
    union {
        Unconditional unconditional;
        BranchReg reg;
        Conditional conditional;
    };
} Branch;

// A Instr(uction) that we can handle is one of:
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
        Branch branch;
    };
} Instr;

#endif
