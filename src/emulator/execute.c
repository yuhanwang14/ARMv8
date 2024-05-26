#include "execute.h"
#include <stdio.h>

extern void execute_dpi(Register *reg, DpImmed dpi);
extern void execute_dpr(Register *reg, DpRegister dpr);
extern void execute_sdt(Register *reg, SdTrans dpr);
extern void execute_ldl(Register *reg, LoadLiteral ldl);
extern void execute_branch(Register *reg, Branch branch);

void execute(Register *reg, Instr *instr) {
    switch (instr->type) {
    case DP_IMMEDIATE_T:
        // DP Immediate
        execute_dpi(reg, instr->dp_immed);
        break;
    case DP_REGISTER_T:
        // DP Register
        execute_dpr(reg, instr->dp_reg);
        break;
    case SINGLE_DATA_TRANSFER_T:
        // Single Data Transfer
        execute_sdt(reg, instr->sing_data_transfer);
        break;
    case LOAD_LITERAL_T:
        // Load Literal
        execute_ldl(reg, instr->load_literal);
        break;
    case BRANCH_T:
        // Branch
        execute_branch(reg, instr->branch);
        break;
    default:
        fprintf(stderr, "Unknown instruction type: %x\n", instr->type);
        exit(EXIT_FAILURE);
    }
}

// TODO
void execute_dpi(Register *reg, DpImmed dpi) {
    switch (dpi.type) {
    case DPI_ARITHMETIC_T: {
        DPIArithmetic instr = dpi.arithmetic;
        break;
    }
    case WIDE_MOVE_T:
        break;
    default:
        fprintf(stderr, "Unknown data processing (Immediate) type: %x\n", dpi.type);
        exit(EXIT_FAILURE);
    }
}
void execute_dpr(Register *reg, DpRegister dpr);
void execute_sdt(Register *reg, SdTrans dpr);
void execute_ldl(Register *reg, LoadLiteral ldl);
void execute_branch(Register *reg, Branch branch);
