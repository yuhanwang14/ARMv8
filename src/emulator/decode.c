#include "decode.h"
#include "stddef.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// the first >> removes all bits before and the & removes all bits after
#define bit_slice(bits, start, size) ((bits >> start) & ((1 << (size + 1)) - 1))
#define nth_bit_set(bits, n) (bits & (1 << (n)))

// Decodes a DP (Immediate) instruction
extern void decode_dpi(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                       Instr *);
// Decodes a DP (Register) instruction
extern void decode_dpr(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                       uint32_t, uint32_t, uint32_t, Instr *);

Instr *decode(uint32_t code) {
    Instr *result = malloc(sizeof(Instr));
    if (!nth_bit_set(code, OP0_OFFSET + 1) &&
        !nth_bit_set(code, OP0_OFFSET + 2)) {
        // DP (Immediate)
        result->type = DP_IMMEDIATE_T;
        uint32_t rd = bit_slice(code, RD_START, RD_SIZE);
        uint32_t operand = bit_slice(code, DPI_OPERAND_START, DPI_OPERAND_SIZE);
        uint32_t opi = bit_slice(code, OPI_START, OPI_SIZE);
        uint32_t opc = bit_slice(code, OPC_START, OPC_SIZE);
        uint32_t sf = nth_bit_set(code, SF_OFFSET);
        decode_dpi(rd, operand, opi, opc, sf, result);
    } else if (nth_bit_set(code, OP0_OFFSET) &&
               nth_bit_set(code, OP0_OFFSET + 2)) {
        // DP (Register)
        result->type = DP_REGISTER_T;
        uint32_t rd = bit_slice(code, RD_START, RD_SIZE);
        uint32_t rn = bit_slice(code, DPR_RN_START, DPR_RN_SIZE);
        uint32_t operand = bit_slice(code, DPR_OPERAND_START, DPR_OPERAND_SIZE);
        uint32_t rm = bit_slice(code, RM_START, RM_SIZE);
        uint32_t opr = bit_slice(code, OPR_START, OPR_SIZE);
        uint32_t m = nth_bit_set(code, M_OFFSET);
        uint32_t opc = bit_slice(code, OPC_START, OPC_SIZE);
        uint32_t sf = nth_bit_set(code, SF_OFFSET);
        decode_dpr(rd, rn, operand, rm, opr, m, opc, sf, result);
    }
    return result;
}

void decode_dpi(uint32_t rd, uint32_t operand, uint32_t opi, uint32_t opc,
                uint32_t sf, Instr *instr) {
    if (opi == ARITHMETIC_OPI) {
        // is arithmetic operation
        instr->dp_immed.type = DPI_ARITHMETIC_T;
        instr->dp_immed.arithmetic.sf = (bool)sf;
        instr->dp_immed.arithmetic.sh = (bool)nth_bit_set(operand, SH_OFFSET);
        instr->dp_immed.arithmetic.opc = (DpArithmeticType)opc;
        instr->dp_immed.arithmetic.imm12 =
            bit_slice(operand, IMM12_START, IMM12_SIZE);
        instr->dp_immed.arithmetic.rn =
            bit_slice(operand, DPI_ARIT_RN_START, DPI_ARIT_RN_SIZE);
        instr->dp_immed.arithmetic.rd = rd;
    } else if (opi == WIDE_MOVE_OPI) {
        // is wide move operation
        instr->dp_immed.type = WIDE_MOVE_T;
        instr->dp_immed.wide_move.sf = (bool)sf;
        instr->dp_immed.wide_move.hw = bit_slice(operand, HW_START, HW_SIZE);
        instr->dp_immed.wide_move.opc = (DpWideMoveType)opc;
        instr->dp_immed.wide_move.imm16 =
            bit_slice(operand, IMM16_START, IMM16_SIZE);
        instr->dp_immed.wide_move.rd = rd;
    } else {
        fprintf(stderr,
                "Failed to decode a DP (Immediate) instruction: Unknown opi "
                "%03b from operand %018b\n",
                opi, operand);
        exit(EXIT_FAILURE);
    }
}

void decode_dpr(uint32_t rd, uint32_t rn, uint32_t operand, uint32_t rm,
                uint32_t opr, uint32_t m, uint32_t opc, uint32_t sf,
                Instr *result) {
    if (m && opr == OPR_MULTIPLY) {
        // Multiply type
        result->dp_reg.type = MULTIPLY_T;
        result->dp_reg.multiply.sf = (bool)sf;
        result->dp_reg.multiply.rd = rd;
        result->dp_reg.multiply.rn = rn;
        result->dp_reg.multiply.rm = rm;
        result->dp_reg.multiply.ra = bit_slice(operand, RA_START, RA_SIZE);
        result->dp_reg.multiply.x = (bool)nth_bit_set(operand, X_OFFSET);
    } else if (!m && !nth_bit_set(opr, ARIT_FLAG)) {
        // Bit-logic type
        result->dp_reg.type = DPR_ARITHMETIC_T;
        result->dp_reg.logical.stype =
            (LogcShiftType)bit_slice(opr, SHIFT_START, SHIFT_SIZE);
        result->dp_reg.logical.N = (bool)nth_bit_set(opr, N_OFFSET);
        result->dp_reg.logical.btype = (BitInstrType)opc;
        result->dp_reg.logical.rd = rd;
        result->dp_reg.logical.rn = rn;
        result->dp_reg.logical.rm = rm;
        result->dp_reg.logical.sf = (bool)sf;
        result->dp_reg.logical.shift = operand;
    } else if (!m && nth_bit_set(opr, ARIT_FLAG) &&
               !nth_bit_set(opr, ARIT_NOT_FLAG)) {
        // Arithmetic type
        result->dp_reg.type = DPR_ARITHMETIC_T;
        result->dp_reg.arithmetic.stype =
            (AritShiftType)bit_slice(opr, SHIFT_START, SHIFT_SIZE);
        result->dp_reg.arithmetic.atype = (DrArithmeticType)opc;
        result->dp_reg.logical.rd = rd;
        result->dp_reg.logical.rn = rn;
        result->dp_reg.logical.rm = rm;
        result->dp_reg.logical.sf = (bool)sf;
        result->dp_reg.arithmetic.shift = operand;
    } else {
        fprintf(stderr,
                "Failed to decode a DP (Register) instruction: Unknown "
                "combination of m = %01b and opr %04b\n",
                m, opr);
        exit(EXIT_FAILURE);
    }
}
