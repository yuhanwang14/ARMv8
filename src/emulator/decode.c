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
        result->type = DP_IMMEDIATE;
        uint32_t rd = bit_slice(code, RD_START, RD_SIZE);
        uint32_t operand = bit_slice(code, DPI_OPERAND_START, DPI_OPERAND_SIZE);
        uint32_t opi = bit_slice(code, OPI_START, OPI_SIZE);
        uint32_t opc = bit_slice(code, OPC_START, OPC_SIZE);
        uint32_t sf = bit_slice(code, SF_START, SF_SIZE);
        decode_dpi(rd, operand, opi, opc, sf, result);
    } else if (nth_bit_set(code, OP0_OFFSET) &&
               nth_bit_set(code, OP0_OFFSET + 2)) {
        // DP (Register)
        result->type = DP_REGISTER;
        uint32_t rd = bit_slice(code, RD_START, RD_SIZE);
        uint32_t rn = bit_slice(code, DPR_RN_START, DPR_RN_SIZE);
        uint32_t operand = bit_slice(code, DPR_OPERAND_START, DPR_OPERAND_SIZE);
        uint32_t rm = bit_slice(code, RM_START, RM_SIZE);
        uint32_t opr = bit_slice(code, OPR_START, OPR_SIZE);
        uint32_t m = bit_slice(code, M_START, M_SIZE);
        uint32_t opc = bit_slice(code, OPC_START, OPC_SIZE);
        uint32_t sf = bit_slice(code, SF_START, SF_SIZE);
        decode_dpr(rd, rn, operand, rm, opr, m, opc, sf, result);
    }
    return result;
}

void decode_dpi(uint32_t rd, uint32_t operand, uint32_t opi, uint32_t opc,
                uint32_t sf, Instr *instr) {
    if (opi == ARITHMETIC_OPI) {
        // is arithmetic operation
        instr->dp_immed.type = ARITHMETIC;
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
        instr->dp_immed.type = WIDE_MOVE;
        instr->dp_immed.wide_move.sf = (bool)sf;
        instr->dp_immed.wide_move.hw = bit_slice(operand, HW_START, HW_SIZE);
        instr->dp_immed.wide_move.opc = (DpWideMoveType)opc;
        instr->dp_immed.wide_move.imm16 =
            bit_slice(operand, IMM16_START, IMM16_SIZE);
        instr->dp_immed.wide_move.rd = rd;
    } else {
        fprintf(stderr, "Unknown opi %03b from operand %018b\n", opi, operand);
        exit(EXIT_FAILURE);
    }
}

void decode_dpr(uint32_t rd, uint32_t rn, uint32_t operand, uint32_t rm,
                uint32_t opr, uint32_t m, uint32_t opc, uint32_t sf,
                Instr *result) {
    // TODO
}