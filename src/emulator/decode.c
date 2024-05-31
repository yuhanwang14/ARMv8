#include "decode.h"
#include "stddef.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// whether the nth bit from op0 is set
#define OP0(n) nth_bit_set(code, OP0_OFFSET + n)

// [yw8123] & [ky723]:
// These constants are the positions of every opprand in the bit code.

static const unsigned OP0_OFFSET = 25;

// consts for DP (Immediate)
static const unsigned RD_START = 0;
static const unsigned RD_SIZE = 5;
static const unsigned DPI_OPERAND_START = RD_START + RD_SIZE;
static const unsigned DPI_OPERAND_SIZE = 18;
static const unsigned OPI_START = DPI_OPERAND_START + DPI_OPERAND_SIZE;
static const unsigned OPI_SIZE = 3;
static const unsigned OPC_START = 29;
static const unsigned OPC_SIZE = 2;
static const unsigned DP_SF_OFFSET = 31;
static const unsigned ARITHMETIC_OPI = 0x2;
static const unsigned WIDE_MOVE_OPI = 0x5;
static const unsigned SH_OFFSET = 17;
static const unsigned DPI_ARIT_RN_START = 0;
static const unsigned DPI_ARIT_RN_SIZE = 5;
static const unsigned IMM12_START = DPI_ARIT_RN_START + DPI_ARIT_RN_SIZE;
static const unsigned IMM12_SIZE = 12;
static const unsigned IMM16_START = 0;
static const unsigned IMM16_SIZE = 16;
static const unsigned HW_START = IMM16_START + IMM16_SIZE;
static const unsigned HW_SIZE = 2;

// consts for DP (Register)
static const unsigned DPR_RN_START = RD_START + RD_SIZE;
static const unsigned DPR_RN_SIZE = 5;
static const unsigned DPR_OPERAND_START = DPR_RN_START + DPR_RN_SIZE;
static const unsigned DPR_OPERAND_SIZE = 6;
static const unsigned RM_START = DPR_OPERAND_START + DPR_OPERAND_SIZE;
static const unsigned RM_SIZE = 5;
static const unsigned OPR_START = RM_START + RM_SIZE;
static const unsigned OPR_SIZE = 4;
static const unsigned M_OFFSET = 28;
static const unsigned OPR_MULTIPLY = 8;
static const unsigned RA_START = 0;
static const unsigned RA_SIZE = 5;
static const unsigned X_OFFSET = 5;
static const unsigned ARIT_FLAG = 3;
static const unsigned ARIT_NOT_FLAG = 0;
static const unsigned SHIFT_START = 1;
static const unsigned SHIFT_SIZE = 2;
static const unsigned N_OFFSET = 0;

// consts for single data transfer
static const unsigned RT_START = 0;
static const unsigned RT_SIZE = 5;
static const unsigned SDT_XN_START = 5;
static const unsigned SDT_XN_SIZE = 5;
static const unsigned OFFSET_START = 10;
static const unsigned OFFSET_SIZE = 12;
static const unsigned L_OFFSET = 22;
static const unsigned U_OFFSET = 24;
static const unsigned LS_SF_OFFSET = 30;
static const unsigned FLAG_OFFSET = 0;
static const unsigned I_OFFSET = 1;
static const unsigned SIMM9_START = 2;
static const unsigned SIMM9_SIZE = 9;
static const unsigned XM_START = 6;
static const unsigned XM_SIZE = 5;

// consts for load literals
static const unsigned LL_SIMM19_START = 5;
static const unsigned LL_SIMM19_SIZE = 19;

// consts for branch
static const unsigned BRANCH_OP_START = 0;
static const unsigned BRANCH_OP_SIZE = 25;
static const unsigned BRANCH_TYPE_START = 26;
static const unsigned BRANCH_TYPE_SIZE = 6;
static const unsigned BR_XN_START = 5;
static const unsigned BR_XN_SIZE = 5;
static const unsigned COND_START = 0;
static const unsigned COND_SIZE = 4;
static const unsigned BR_SIMM19_START = 5;
static const unsigned BR_SIMM19_SIZE = 19;
static const unsigned BR_UNCOND_OFF_SIZE = 21;
static const unsigned BR_COND_OFF_SIZE = 28;

static uint32_t bit_slice(uint32_t bits, unsigned start, unsigned size) {
    // getting the bits in [start..start+size)
    return ((bits >> start) & (((uint32_t)1 << size) - 1));
}

static bool nth_bit_set(uint32_t bits, unsigned n) {
    // whether or not the nth bit of bits is set
    return (bool)(bits & ((uint32_t)1 << (n)));
}

static void decode_dpi(uint32_t rd, uint32_t operand, uint32_t opi, uint32_t opc, uint32_t sf,
                       Instr *result) {
    if (opi == ARITHMETIC_OPI) {
        // arithmetic type
        result->dp_immed.type = DPI_ARITHMETIC_T;
        result->dp_immed.arithmetic.sf = (bool)sf;
        result->dp_immed.arithmetic.sh = (bool)nth_bit_set(operand, SH_OFFSET);
        result->dp_immed.arithmetic.atype = (ArithmeticType)opc;
        result->dp_immed.arithmetic.imm12 = bit_slice(operand, IMM12_START, IMM12_SIZE);
        result->dp_immed.arithmetic.rn = bit_slice(operand, DPI_ARIT_RN_START, DPI_ARIT_RN_SIZE);
        result->dp_immed.arithmetic.rd = rd;
    } else if (opi == WIDE_MOVE_OPI) {
        // wide move type
        result->dp_immed.type = WIDE_MOVE_T;
        result->dp_immed.wide_move.sf = (bool)sf;
        result->dp_immed.wide_move.hw = bit_slice(operand, HW_START, HW_SIZE);
        result->dp_immed.wide_move.mtype = (DpWideMoveType)opc;
        result->dp_immed.wide_move.imm16 = bit_slice(operand, IMM16_START, IMM16_SIZE);
        result->dp_immed.wide_move.rd = rd;
    } else {
        fprintf(stderr,
                "Failed to decode a DP (Immediate) instruction: Unknown opi "
                "0x%x from operand %x\n",
                opi, operand);
        free(result);
        exit(EXIT_FAILURE);
    }
}

// Decodes a DP (Register) instruction
static void decode_dpr(uint32_t rd, uint32_t rn, uint32_t operand, uint32_t rm, uint32_t opr,
                       uint32_t m, uint32_t opc, uint32_t sf, Instr *result) {
    if (m && opr == OPR_MULTIPLY) {
        // multiply type
        result->dp_reg.type = MULTIPLY_T;
        result->dp_reg.multiply.sf = (bool)sf;
        result->dp_reg.multiply.rd = rd;
        result->dp_reg.multiply.rn = rn;
        result->dp_reg.multiply.rm = rm;
        result->dp_reg.multiply.ra = bit_slice(operand, RA_START, RA_SIZE);
        result->dp_reg.multiply.x = (bool)nth_bit_set(operand, X_OFFSET);
    } else if (!m && !nth_bit_set(opr, ARIT_FLAG)) {
        // bit-logic type
        result->dp_reg.type = BIT_LOGIC_T;
        result->dp_reg.logical.stype = (LogcShiftType)bit_slice(opr, SHIFT_START, SHIFT_SIZE);
        result->dp_reg.logical.N = (bool)nth_bit_set(opr, N_OFFSET);
        result->dp_reg.logical.btype = (BitInstrType)opc;
        result->dp_reg.logical.rd = rd;
        result->dp_reg.logical.rn = rn;
        result->dp_reg.logical.rm = rm;
        result->dp_reg.logical.sf = (bool)sf;
        result->dp_reg.logical.shift = operand;
    } else if (!m && nth_bit_set(opr, ARIT_FLAG) && !nth_bit_set(opr, ARIT_NOT_FLAG)) {
        // arithmetic type
        result->dp_reg.type = DPR_ARITHMETIC_T;
        result->dp_reg.arithmetic.stype = (AritShiftType)bit_slice(opr, SHIFT_START, SHIFT_SIZE);
        result->dp_reg.arithmetic.atype = (ArithmeticType)opc;
        result->dp_reg.logical.rd = rd;
        result->dp_reg.logical.rn = rn;
        result->dp_reg.logical.rm = rm;
        result->dp_reg.logical.sf = (bool)sf;
        result->dp_reg.arithmetic.shift = operand;
    } else {
        fprintf(stderr,
                "Failed to decode a DP (Register) instruction: Unknown "
                "combination of m = 0x%x and opr 0x%x\n",
                m, opr);
        free(result);
        exit(EXIT_FAILURE);
    }
}

// Decodes a single data transfer instruction
static void decode_sdt(uint32_t rt, uint64_t xn, uint32_t offset, uint32_t l, uint32_t u,
                       uint32_t sf, Instr *result) {
    if (u) {
        // unsigned type
        result->sing_data_transfer.type = UNSIGN_T;
        result->sing_data_transfer.usigned.imm12 = offset;
        result->sing_data_transfer.usigned.xn = xn;
        result->sing_data_transfer.usigned.rt = rt;
        result->sing_data_transfer.usigned.sf = (bool)sf;
        result->sing_data_transfer.usigned.L = l;
    } else if (nth_bit_set(offset, FLAG_OFFSET)) {
        // pre/post-index type
        result->sing_data_transfer.type = PRE_POST_INDEX_T;
        result->sing_data_transfer.pre_post_index.itype =
            (nth_bit_set(offset, I_OFFSET)) ? PRE_INDEX : POST_INDEX;
        result->sing_data_transfer.pre_post_index.L = (bool)l;
        result->sing_data_transfer.pre_post_index.rt = rt;
        result->sing_data_transfer.pre_post_index.sf = (bool)sf;
        result->sing_data_transfer.pre_post_index.simm9 =
            bit_slice(offset, SIMM9_START, SIMM9_SIZE);
        result->sing_data_transfer.pre_post_index.xn = xn;
    } else if (!nth_bit_set(offset, FLAG_OFFSET)) {
        // register type
        result->sing_data_transfer.type = SD_REGISTER_T;
        result->sing_data_transfer.reg.xm = bit_slice(offset, XM_START, XM_SIZE);
        result->sing_data_transfer.reg.L = (bool)l;
        result->sing_data_transfer.reg.rt = rt;
        result->sing_data_transfer.reg.sf = (bool)sf;
        result->sing_data_transfer.reg.xn = xn;
    } else {
        fprintf(stderr,
                "Failed to decode a single data transfer instruction: Unknown "
                "combination of u = 0x%x and offset 0x%x\n",
                u, offset);
        free(result);
        exit(EXIT_FAILURE);
    }
}

// Decodes a branch instruction
static void decode_branch(uint32_t operand, uint32_t type, Instr *result) {
    if (type == UNCONDITIONAL_T) {
        // unconditional type
        result->branch.type = UNCONDITIONAL_T;
        result->branch.unconditional.offset = sign_extend((int64_t)operand, BR_COND_OFF_SIZE);
    } else if (type == BR_REGISTER_T) {
        // register type
        result->branch.type = BR_REGISTER_T;
        result->branch.reg.xn = bit_slice(operand, BR_XN_START, BR_XN_SIZE);
    } else if (type == CONDITIONAL_T) {
        // conditional type
        result->branch.type = CONDITIONAL_T;
        result->branch.conditional.cond = bit_slice(operand, COND_START, COND_SIZE);
        int64_t offset_nonextended =
            (int64_t)(bit_slice(operand, BR_SIMM19_START, BR_SIMM19_SIZE) << 2);
        result->branch.conditional.offset = sign_extend(offset_nonextended, BR_UNCOND_OFF_SIZE);
    } else {
        fprintf(stderr,
                "Failed to decode a branch instruction: Unknown combination of type = 0x%x\n",
                type);
        free(result);
        exit(EXIT_FAILURE);
    }
}

void decode(uint32_t code, Instr *result) {
    // we read op0 to determine which category of instruction we have encountered (spec p8)
    // for each big case we slice the arguments out and process it further in corresponding
    // functions with the exception of load literal because it only has one case, hence we processed
    // it inline
    if (!OP0(1) && !OP0(2)) {
        // DP (Immediate)
        result->type = DP_IMMEDIATE_T;
        uint32_t rd = bit_slice(code, RD_START, RD_SIZE);
        uint32_t operand = bit_slice(code, DPI_OPERAND_START, DPI_OPERAND_SIZE);
        uint32_t opi = bit_slice(code, OPI_START, OPI_SIZE);
        uint32_t opc = bit_slice(code, OPC_START, OPC_SIZE);
        uint32_t sf = nth_bit_set(code, DP_SF_OFFSET);
        decode_dpi(rd, operand, opi, opc, sf, result);
    } else if (OP0(0) && OP0(2)) {
        // DP (Register)
        result->type = DP_REGISTER_T;
        uint32_t rd = bit_slice(code, RD_START, RD_SIZE);
        uint32_t rn = bit_slice(code, DPR_RN_START, DPR_RN_SIZE);
        uint32_t operand = bit_slice(code, DPR_OPERAND_START, DPR_OPERAND_SIZE);
        uint32_t rm = bit_slice(code, RM_START, RM_SIZE);
        uint32_t opr = bit_slice(code, OPR_START, OPR_SIZE);
        uint32_t m = nth_bit_set(code, M_OFFSET);
        uint32_t opc = bit_slice(code, OPC_START, OPC_SIZE);
        uint32_t sf = nth_bit_set(code, DP_SF_OFFSET);
        decode_dpr(rd, rn, operand, rm, opr, m, opc, sf, result);
    } else if (!OP0(0) && OP0(2) && OP0(3) && OP0(4)) {
        // Single Data Transfer
        result->type = SINGLE_DATA_TRANSFER_T;
        uint32_t rt = bit_slice(code, RT_START, RT_SIZE);
        uint64_t xn = bit_slice(code, SDT_XN_START, SDT_XN_SIZE);
        uint32_t offset = bit_slice(code, OFFSET_START, OFFSET_SIZE);
        uint32_t l = nth_bit_set(code, L_OFFSET);
        uint32_t u = nth_bit_set(code, U_OFFSET);
        uint32_t sf = nth_bit_set(code, LS_SF_OFFSET);
        decode_sdt(rt, xn, offset, l, u, sf, result);
    } else if (!OP0(0) && OP0(2) && OP0(3)) {
        // Load From Literal
        result->type = LOAD_LITERAL_T;
        result->load_literal.rt = bit_slice(code, RT_START, RT_SIZE);
        result->load_literal.sf = nth_bit_set(code, LS_SF_OFFSET);
        result->load_literal.simm19 = bit_slice(code, LL_SIMM19_START, LL_SIMM19_SIZE);
    } else if (OP0(1) && OP0(3)) {
        // Branch
        result->type = BRANCH_T;
        uint32_t operand = bit_slice(code, BRANCH_OP_START, BRANCH_OP_SIZE);
        uint32_t type = bit_slice(code, BRANCH_TYPE_START, BRANCH_TYPE_SIZE);
        decode_branch(operand, type, result);
    } else {
        fprintf(stderr, "Failed to recognize 0x%x as any instruction", code);
        free(result);
        exit(EXIT_FAILURE);
    }
}
