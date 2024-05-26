#include "execute.h"
#include <stdint.h>
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
        fprintf(stderr, "Unknown instruction type: 0x%x\n", instr->type);
        exit(EXIT_FAILURE);
    }
}

#define R32(n) ((uint32_t *)reg->g_reg)[(n) * 2]
#define R32_cls_upper(n) ((uint32_t *)reg->g_reg)[(n) * 2 + 1] = 0
#define R64(n) reg->g_reg[n]
#define sgn64(n) (bool)((n >> 63) & 1)
#define sgn32(n) (bool)((n >> 31) & 1)
#define R64_16(n, shift) ((uint16_t *)reg->g_reg)[(n) * 4 + shift]
#define R32_16(n, shift) R64_16(n, shift)

void execute_dpi(Register *reg, DpImmed dpi) {
    switch (dpi.type) {
    case DPI_ARITHMETIC_T: {
        DPIArithmetic instr = dpi.arithmetic;
        uint32_t op2 = instr.sh ? instr.imm12 << 12 : instr.imm12;
        switch (instr.atype) {
        case ADD:
            if (instr.sf) {
                // 64 bit mode
                R64(instr.rd) = R64(instr.rn) + (uint64_t)op2;
            } else {
                // 32 bit mode
                R32(instr.rd) = R32(instr.rn) + op2;
                R32_cls_upper(instr.rd);
            }
            break;
        case ADDS:
            if (instr.sf) {
                // 64 bit mode
                uint64_t u_result;
                int64_t i_result;
                // ironically the function is likely implemented using these instructions
                // update unsigned overflow indicator flags
                reg->PSTATE->C = __builtin_add_overflow(R64(instr.rn), (uint64_t)op2, &u_result);
                // update signed overflow indicator flags
                reg->PSTATE->V = __builtin_add_overflow((int64_t)R64(instr.rn), (int64_t)op2, &i_result);
                R64(instr.rd) = u_result;
                reg->PSTATE->N = sgn64(i_result);
                reg->PSTATE->Z = u_result == 0;
            } else {
                // 32 bit mode
                uint32_t u_result;
                int32_t i_result;
                reg->PSTATE->C = __builtin_add_overflow(R32(instr.rn), op2, &u_result);
                reg->PSTATE->V = __builtin_add_overflow((int32_t)R32(instr.rn), (int32_t)op2, &i_result);
                R32_cls_upper(instr.rd);
                R64(instr.rd) = u_result;
                reg->PSTATE->N = sgn32(i_result);
                reg->PSTATE->Z = u_result == 0;
            }
            break;
        case SUB:
            if (instr.sf) {
                // 64 bit mode
                R64(instr.rd) = R64(instr.rn) - (uint64_t)op2;
            } else {
                // 32 bit mode
                R32(instr.rd) = R32(instr.rn) - op2;
                R32_cls_upper(instr.rd);
            }
            break;
        case SUBS:
            if (instr.sf) {
                // 64 bit mode
                uint64_t u_result;
                int64_t i_result;
                // ironically the function is likely implemented using these instructions
                // update unsigned overflow indicator flags
                reg->PSTATE->C = __builtin_sub_overflow(R64(instr.rn), (uint64_t)op2, &u_result);
                // update signed overflow indicator flags
                reg->PSTATE->V = __builtin_sub_overflow((int64_t)R64(instr.rn), (int64_t)op2, &i_result);
                R64(instr.rd) = u_result;
                reg->PSTATE->N = sgn64(i_result);
                reg->PSTATE->Z = u_result == 0;
            } else {
                // 32 bit mode
                uint32_t u_result;
                int32_t i_result;
                reg->PSTATE->C = __builtin_sub_overflow(R32(instr.rn), op2, &u_result);
                reg->PSTATE->V = __builtin_sub_overflow((int32_t)R32(instr.rn), (int32_t)op2, &i_result);
                R32_cls_upper(instr.rd);
                R64(instr.rd) = u_result;
                reg->PSTATE->N = sgn32(i_result);
                reg->PSTATE->Z = u_result == 0;
            }
            break;
        }
        break;
    }
    case WIDE_MOVE_T: {
        WideMove instr = dpi.wide_move;
        uint32_t op = instr.imm16 << (instr.hw * 16);
        switch (instr.mtype) {
        case MOVN: {
            if (instr.sf) {
                // 64 bit mode
                R64(instr.rd) = ~((uint64_t)op);
            } else {
                // 32 bit mode
                R32(instr.rd) = ~op;
                R32_cls_upper(instr.rd);
            }
            break;
        }
        case MOVZ: {
            if (instr.sf) {
                // 64 bit mode
                R64(instr.rd) = (uint64_t)op;
            } else {
                // 32 bit mode
                R32(instr.rd) = op;
                R32_cls_upper(instr.rd);
            }
            break;
        }
        case MOVK: {
            if (instr.sf) {
                // 64 bit mode
                R64_16(instr.rd, instr.hw) = instr.imm16;
            } else {
                // 32 bit mode
                R64_16(instr.rd, instr.hw) = instr.imm16;
                R32_cls_upper(instr.rd);
            }
            break;
        }
        default:
            fprintf(stderr, "Unknown wide move type: 0x%x\n", instr.mtype);
            exit(EXIT_FAILURE);
            break;
        }
        break;
    }
    default:
        fprintf(stderr, "Unknown data processing (Immediate) type: 0x%x\n", dpi.type);
        exit(EXIT_FAILURE);
    }
}
void execute_dpr(Register *reg, DpRegister dpr) {
    // TODO: ky723
}
void execute_sdt(Register *reg, SdTrans dpr) {
    // TODO: yw8123
}
void execute_ldl(Register *reg, LoadLiteral ldl) {
    // TODO: yw8123
}
void execute_branch(Register *reg, Branch branch) {
    switch (branch.type) {
    case UNCONDITIONAL_T: {
        Unconditional instr = branch.unconditional;
        reg->PC += instr.simm26;
        break;
    }
    case BR_REGISTER_T: {
        BranchReg instr = branch.reg;
        reg->PC = R64(instr.xn);
        break;
    }
    case CONDITIONAL_T: {
        Conditional instr = branch.conditional;
        PState ps = *reg->PSTATE;
        switch (instr.cond) {
        case EQ:
            if (ps.Z) {
                reg->PC += instr.simm19;
            }
            break;
        case NE:
            if (!ps.Z) {
                reg->PC += instr.simm19;
            }
            break;
        case GE:
            if (ps.N == ps.V) {
                reg->PC += instr.simm19;
            }
            break;
        case LT:
            if (ps.N != ps.V) {
                reg->PC += instr.simm19;
            }
            break;
        case GT:
            if (!ps.Z && ps.N == ps.V) {
                reg->PC += instr.simm19;
            }
            break;
        case LE:
            if (!(!ps.Z && ps.N == ps.V)) {
                reg->PC += instr.simm19;
            }
            break;
        case AL:
            if (true) {
                reg->PC += instr.simm19;
            }
            break;
        default:
            fprintf(stderr, "Unknown branch conditional type: 0x%x\n", instr.cond);
            exit(EXIT_FAILURE);
            break;
        }
        break;
    }
    default:
        fprintf(stderr, "Unknown branch type: 0x%x\n", branch.type);
        exit(EXIT_FAILURE);
        break;
    }
}
