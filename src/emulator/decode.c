#include "decode.h"
#include "stddef.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const uint32_t OP0_OFFSET = 25;

// consts for DP (Immediate)
const uint32_t RD_START = 0;
const uint32_t RD_SIZE = 5;
const uint32_t DPI_OPERAND_START = RD_START + RD_SIZE;
const uint32_t DPI_OPERAND_SIZE = 18;
const uint32_t OPI_START = DPI_OPERAND_START + DPI_OPERAND_SIZE;
const uint32_t OPI_SIZE = 3;
const uint32_t OPC_START = 29;
const uint32_t OPC_SIZE = 2;
const uint32_t DP_SF_OFFSET = 31;
const uint32_t ARITHMETIC_OPI = 0x2;
const uint32_t WIDE_MOVE_OPI = 0x5;
const uint32_t SH_OFFSET = 17;
const uint32_t DPI_ARIT_RN_START = 0;
const uint32_t DPI_ARIT_RN_SIZE = 5;
const uint32_t IMM12_START = DPI_ARIT_RN_START + DPI_ARIT_RN_SIZE;
const uint32_t IMM12_SIZE = 12;
const uint32_t IMM16_START = 0;
const uint32_t IMM16_SIZE = 16;
const uint32_t HW_START = IMM16_START + IMM16_SIZE;
const uint32_t HW_SIZE = 2;

// consts for DP (Register)
const uint32_t DPR_RN_START = RD_START + RD_SIZE;
const uint32_t DPR_RN_SIZE = 5;
const uint32_t DPR_OPERAND_START = DPR_RN_START + DPR_RN_SIZE;
const uint32_t DPR_OPERAND_SIZE = 6;
const uint32_t RM_START = DPR_OPERAND_START + DPR_OPERAND_SIZE;
const uint32_t RM_SIZE = 5;
const uint32_t OPR_START = RM_START + RM_SIZE;
const uint32_t OPR_SIZE = 4;
const uint32_t M_OFFSET = 28;
const uint32_t OPR_MULTIPLY = 8;
const uint32_t RA_START = 0;
const uint32_t RA_SIZE = 5;
const uint32_t X_OFFSET = 5;
const uint32_t ARIT_FLAG = 3;
const uint32_t ARIT_NOT_FLAG = 0;
const uint32_t SHIFT_START = 1;
const uint32_t SHIFT_SIZE = 2;
const uint32_t N_OFFSET = 0;

// consts for single data transfer
const uint32_t RT_START = 0;
const uint32_t RT_SIZE = 5;
const uint32_t SDT_XN_START = 5;
const uint32_t SDT_XN_SIZE = 5;
const uint32_t OFFSET_START = 10;
const uint32_t OFFSET_SIZE = 12;
const uint32_t L_OFFSET = 22;
const uint32_t U_OFFSET = 24;
const uint32_t LS_SF_OFFSET = 30;
const uint32_t FLAG_OFFSET = 0;
const uint32_t I_OFFSET = 1;
const uint32_t SIMM9_START = 2;
const uint32_t SIMM9_SIZE = 9;
const uint32_t XM_START = 6;
const uint32_t XM_SIZE = 5;

// consts for load literals
const uint32_t LL_SIMM19_START = 5;
const uint32_t LL_SIMM19_SIZE = 19;

// consts for branch
const uint32_t BRANCH_OP_START = 0;
const uint32_t BRANCH_OP_SIZE = 25;
const uint32_t BRANCH_TYPE_START = 26;
const uint32_t BRANCH_TYPE_SIZE = 6;
const uint32_t BR_XN_START = 5;
const uint32_t BR_XN_SIZE = 5;
const uint32_t COND_START = 0;
const uint32_t COND_SIZE = 4;
const uint32_t BR_SIMM19_START = 5;
const uint32_t BR_SIMM19_SIZE = 19;

// Both macros correspont to 0..31 in the spec
// the first >> removes all bits before and the & removes all bits after
#define bit_slice(bits, start, size) ((bits >> start) & (((uint32_t)1 << size) - 1))
#define nth_bit_set(bits, n) (bits & ((uint32_t)1 << (n)))

// Decodes a DP (Immediate) instruction
extern void decode_dpi(uint32_t rd, uint32_t operand, uint32_t opi, uint32_t opc, uint32_t sf, Instr *result);

// Decodes a DP (Register) instruction
extern void decode_dpr(uint32_t rd, uint32_t rn, uint32_t operand, uint32_t rm, uint32_t opr, uint32_t m,
                       uint32_t opc, uint32_t sf, Instr *result);

// Decodes a single data transfer instruction
extern void decode_sdt(uint32_t rt, uint64_t xn, uint32_t offset, uint32_t l, uint32_t u, uint32_t sf,
                       Instr *result);

// Decodes a branch instruction
extern void decode_branch(uint32_t operand, uint32_t type, Instr *result);

Instr *decode(uint32_t code) {
    Instr *result = malloc(sizeof(Instr));
    if (!nth_bit_set(code, OP0_OFFSET + 1) && !nth_bit_set(code, OP0_OFFSET + 2)) {
        // DP (Immediate)
        result->type = DP_IMMEDIATE_T;
        uint32_t rd = bit_slice(code, RD_START, RD_SIZE);
        uint32_t operand = bit_slice(code, DPI_OPERAND_START, DPI_OPERAND_SIZE);
        uint32_t opi = bit_slice(code, OPI_START, OPI_SIZE);
        uint32_t opc = bit_slice(code, OPC_START, OPC_SIZE);
        uint32_t sf = nth_bit_set(code, DP_SF_OFFSET);
        printf("Type: DP (Immediate)\n");
#if __GNUC__ == 14
        printf("    rd: %032b\n", rd);
        printf("    operand: %032b\n", operand);
        printf("    opi: %032b\n", opi);
        printf("    opc: %032b\n", opc);
        printf("    sf: %032b\n", sf);
#endif
        decode_dpi(rd, operand, opi, opc, sf, result);
    } else if (nth_bit_set(code, OP0_OFFSET) && nth_bit_set(code, OP0_OFFSET + 2)) {
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
        printf("Type: DP (Register)\n");
#if __GNUC__ == 14
        printf("    rd: %032b\n", rd);
        printf("    rn: %032b\n", rn);
        printf("    operand: %032b\n", operand);
        printf("    rm: %032b\n", rm);
        printf("    opr: %032b\n", opr);
        printf("    opc: %032b\n", opc);
        printf("    sf: %032b\n", sf);
#endif
        decode_dpr(rd, rn, operand, rm, opr, m, opc, sf, result);
    } else if (!nth_bit_set(code, OP0_OFFSET) && nth_bit_set(code, OP0_OFFSET + 2) &&
               nth_bit_set(code, OP0_OFFSET + 3) && nth_bit_set(code, OP0_OFFSET + 4)) {
        // Single Data Transfer
        printf("Type: Single Data Transfer\n");
        result->type = SINGLE_DATA_TRANSFER_T;
        uint32_t rt = bit_slice(code, RT_START, RT_SIZE);
        uint64_t xn = bit_slice(code, SDT_XN_START, SDT_XN_SIZE);
        uint32_t offset = bit_slice(code, OFFSET_START, OFFSET_SIZE);
        uint32_t l = nth_bit_set(code, L_OFFSET);
        uint32_t u = nth_bit_set(code, U_OFFSET);
        uint32_t sf = nth_bit_set(code, LS_SF_OFFSET);
        decode_sdt(rt, xn, offset, l, u, sf, result);
    } else if (!nth_bit_set(code, OP0_OFFSET) && nth_bit_set(code, OP0_OFFSET + 2) &&
               nth_bit_set(code, OP0_OFFSET + 3)) {
        // Load From Literal
        printf("Type: Load From Literal\n");
        result->type = LOAD_LITERAL_T;
        result->load_literal.rt = bit_slice(code, RT_START, RT_SIZE);
        result->load_literal.sf = nth_bit_set(code, LS_SF_OFFSET);
        result->load_literal.simm19 = bit_slice(code, LL_SIMM19_START, LL_SIMM19_SIZE);
    } else if (nth_bit_set(code, OP0_OFFSET + 1) && nth_bit_set(code, OP0_OFFSET + 3)) {
        // Branch
        printf("Type: Branch\n");
        result->type = BRANCH_T;
        uint32_t operand = bit_slice(code, BRANCH_OP_START, BRANCH_OP_SIZE);
        uint32_t type = bit_slice(code, BRANCH_TYPE_START, BRANCH_TYPE_SIZE);
        decode_branch(operand, type, result);
    } else {
        fprintf(stderr, "Failed to recognize 0x%x as any instruction", code);
        free(result);
        exit(EXIT_FAILURE);
    }
    return result;
}

void decode_dpi(uint32_t rd, uint32_t operand, uint32_t opi, uint32_t opc, uint32_t sf, Instr *result) {
    if (opi == ARITHMETIC_OPI) {
        // is arithmetic operation
        result->dp_immed.type = DPI_ARITHMETIC_T;
        result->dp_immed.arithmetic.sf = (bool)sf;
        result->dp_immed.arithmetic.sh = (bool)nth_bit_set(operand, SH_OFFSET);
        result->dp_immed.arithmetic.atype = (ArithmeticType)opc;
        result->dp_immed.arithmetic.imm12 = bit_slice(operand, IMM12_START, IMM12_SIZE);
        result->dp_immed.arithmetic.rn = bit_slice(operand, DPI_ARIT_RN_START, DPI_ARIT_RN_SIZE);
        result->dp_immed.arithmetic.rd = rd;
        printf("    Type: Arithmetic\n");
        printf("    opc: %u\n", opc);
        printf("    sh: %u\n", result->dp_immed.arithmetic.sh);
#if __GNUC__ == 14
#endif
    } else if (opi == WIDE_MOVE_OPI) {
        // is wide move operation
        printf("    Type: Wide move\n");
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

void decode_dpr(uint32_t rd, uint32_t rn, uint32_t operand, uint32_t rm, uint32_t opr, uint32_t m,
                uint32_t opc, uint32_t sf, Instr *result) {
    if (m && opr == OPR_MULTIPLY) {
        // Multiply type
        result->dp_reg.type = MULTIPLY_T;
        result->dp_reg.multiply.sf = (bool)sf;
        result->dp_reg.multiply.rd = rd;
        result->dp_reg.multiply.rn = rn;
        result->dp_reg.multiply.rm = rm;
        result->dp_reg.multiply.ra = bit_slice(operand, RA_START, RA_SIZE);
        result->dp_reg.multiply.x = (bool)nth_bit_set(operand, X_OFFSET);
        printf("DPR Type: Multiply type\n");
    } else if (!m && !nth_bit_set(opr, ARIT_FLAG)) {
        // Bit-logic type
        result->dp_reg.type = BIT_LOGIC_T;
        result->dp_reg.logical.stype = (LogcShiftType)bit_slice(opr, SHIFT_START, SHIFT_SIZE);
        result->dp_reg.logical.N = (bool)nth_bit_set(opr, N_OFFSET);
        result->dp_reg.logical.btype = (BitInstrType)opc;
        result->dp_reg.logical.rd = rd;
        result->dp_reg.logical.rn = rn;
        result->dp_reg.logical.rm = rm;
        result->dp_reg.logical.sf = (bool)sf;
        result->dp_reg.logical.shift = operand;
        printf("DPR Type: Bit logic type\n");
        printf("    stype: %d\n", (LogcShiftType)result->dp_reg.logical.stype);
        printf("    N: %d\n", result->dp_reg.logical.N);
        printf("    btype: %d\n", result->dp_reg.logical.btype);
        printf("    rd: %u\n", result->dp_reg.logical.rd);
        printf("    rn: %u\n", result->dp_reg.logical.rn);
        printf("    rm: %u\n", result->dp_reg.logical.rm);
        printf("    sf: %u\n", result->dp_reg.logical.sf);
        printf("    shift: %u\n", result->dp_reg.logical.shift);
    } else if (!m && nth_bit_set(opr, ARIT_FLAG) && !nth_bit_set(opr, ARIT_NOT_FLAG)) {
        // Arithmetic type
        result->dp_reg.type = DPR_ARITHMETIC_T;
        result->dp_reg.arithmetic.stype = (AritShiftType)bit_slice(opr, SHIFT_START, SHIFT_SIZE);
        result->dp_reg.arithmetic.atype = (ArithmeticType)opc;
        result->dp_reg.logical.rd = rd;
        result->dp_reg.logical.rn = rn;
        result->dp_reg.logical.rm = rm;
        result->dp_reg.logical.sf = (bool)sf;
        result->dp_reg.arithmetic.shift = operand;
        printf("DPR Type: Arithmetic type\n");
    } else {
        fprintf(stderr,
                "Failed to decode a DP (Register) instruction: Unknown "
                "combination of m = 0x%x and opr 0x%x\n",
                m, opr);
        free(result);
        exit(EXIT_FAILURE);
    }
}

void decode_sdt(uint32_t rt, uint64_t xn, uint32_t offset, uint32_t l, uint32_t u, uint32_t sf,
                Instr *result) {
    if (u == 0x1) {
        // is unsigned type
        result->sing_data_transfer.type = UNSIGN_T;
        result->sing_data_transfer.usigned.imm12 = offset;
        result->sing_data_transfer.usigned.xn = xn;
        result->sing_data_transfer.usigned.rt = rt;
        result->sing_data_transfer.usigned.sf = (bool)sf;
        result->sing_data_transfer.usigned.L = l;
    } else if (nth_bit_set(offset, FLAG_OFFSET)) {
        // is pre/post-index type
        result->sing_data_transfer.type = PRE_POST_INDEX_T;
        result->sing_data_transfer.pre_post_index.itype =
            (nth_bit_set(offset, I_OFFSET) == (uint32_t)1) ? PRE_INDEX : POST_INDEX;
        result->sing_data_transfer.pre_post_index.L = (bool)l;
        result->sing_data_transfer.pre_post_index.rt = rt;
        result->sing_data_transfer.pre_post_index.sf = (bool)sf;
        result->sing_data_transfer.pre_post_index.simm9 = bit_slice(offset, SIMM9_START, SIMM9_SIZE);
        result->sing_data_transfer.pre_post_index.xn = xn;
    } else if (!nth_bit_set(offset, FLAG_OFFSET)) {
        // is register type
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

void decode_branch(uint32_t operand, uint32_t type, Instr *result) {
    if (type == UNCONDITIONAL_T) {
        // unconditional
        result->branch.type = UNCONDITIONAL_T;
        result->branch.unconditional.simm26 = operand;
    } else if (type == BR_REGISTER_T) {
        // register
        result->branch.type = BR_REGISTER_T;
        result->branch.reg.xn = bit_slice(operand, BR_XN_START, BR_XN_SIZE);
    } else if (type == CONDITIONAL_T) {
        // conditional
        result->branch.type = CONDITIONAL_T;
        result->branch.conditional.cond = bit_slice(operand, COND_START, COND_SIZE);
        result->branch.conditional.simm19 = bit_slice(operand, BR_SIMM19_START, BR_SIMM19_SIZE);
    } else {

        fprintf(stderr, "Failed to decode a branch instruction: Unknown combination of type = 0x%x\n", type);
        free(result);
        exit(EXIT_FAILURE);
    }
}
