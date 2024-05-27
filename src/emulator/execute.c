#include "execute.h"
#include <stdint.h>
#include <stdio.h>

const uint32_t MULT_ZERO_REG = 0x1F;

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

void execute_arit_instr(Register *reg, ArithmeticType atype, bool sf, uint32_t rd, uint32_t rn,
                        uint64_t op2) {
    uint32_t op2_32 = (uint32_t)op2;
    switch (atype) {
    case ADD:
        if (sf) {
            // 64 bit mode
            R64(rd) = R64(rn) + op2;
        } else {
            // 32 bit mode
            R32(rd) = R32(rn) + op2_32;
            R32_cls_upper(rd);
        }
        break;
    case ADDS:
        if (sf) {
            // 64 bit mode
            uint64_t u_result;
            int64_t i_result;
            // ironically the function is likely implemented using these instructions
            // update unsigned overflow indicator flags
            reg->PSTATE->C = __builtin_add_overflow(R64(rn), op2, &u_result);
            // update signed overflow indicator flags
            reg->PSTATE->V = __builtin_add_overflow((int64_t)R64(rn), (int64_t)op2, &i_result);
            R64(rd) = u_result;
            reg->PSTATE->N = sgn64(i_result);
            reg->PSTATE->Z = u_result == 0;
        } else {
            // 32 bit mode
            uint32_t u_result;
            int32_t i_result;
            reg->PSTATE->C = __builtin_add_overflow(R32(rn), op2_32, &u_result);
            reg->PSTATE->V = __builtin_add_overflow((int32_t)R32(rn), (int32_t)op2_32, &i_result);
            R32_cls_upper(rd);
            R64(rd) = u_result;
            reg->PSTATE->N = sgn32(i_result);
            reg->PSTATE->Z = u_result == 0;
        }
        break;
    case SUB:
        if (sf) {
            // 64 bit mode
            R64(rd) = R64(rn) - op2;
        } else {
            // 32 bit mode
            R32(rd) = R32(rn) - op2_32;
            R32_cls_upper(rd);
        }
        break;
    case SUBS:
        if (sf) {
            // 64 bit mode
            uint64_t u_result;
            int64_t i_result;
            // ironically the function is likely implemented using these instructions
            // update unsigned overflow indicator flags
            reg->PSTATE->C = __builtin_sub_overflow(R64(rn), op2, &u_result);
            // update signed overflow indicator flags
            reg->PSTATE->V = __builtin_sub_overflow((int64_t)R64(rn), (int64_t)op2, &i_result);
            R64(rd) = u_result;
            reg->PSTATE->N = sgn64(i_result);
            reg->PSTATE->Z = u_result == 0;
        } else {
            // 32 bit mode
            uint32_t u_result;
            int32_t i_result;
            reg->PSTATE->C = __builtin_sub_overflow(R32(rn), op2_32, &u_result);
            reg->PSTATE->V = __builtin_sub_overflow((int32_t)R32(rn), (int32_t)op2_32, &i_result);
            R32_cls_upper(rd);
            R64(rd) = u_result;
            reg->PSTATE->N = sgn32(i_result);
            reg->PSTATE->Z = u_result == 0;
        }
        break;
    default:
        fprintf(stderr, "Unknown arithmetic type: 0x%x\n", atype);
        exit(EXIT_FAILURE);
        break;
    }
}

void execute_dpi(Register *reg, DpImmed dpi) {
    switch (dpi.type) {
    case DPI_ARITHMETIC_T: {
        DPIArithmetic instr = dpi.arithmetic;
        uint32_t op2 = instr.sh ? instr.imm12 << 12 : instr.imm12;
        execute_arit_instr(reg, instr.atype, instr.sf, instr.rd, instr.rn, (uint64_t)op2);
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
    switch (dpr.type) {
    case DPR_ARITHMETIC_T: {
        DPRArithmetic instr = dpr.arithmetic;
        if (instr.sf) {
            // 64 bit mode
            uint64_t op2;
            switch (instr.stype) {
            case A_LSL_T:
                op2 = R64(instr.rm) << instr.shift;
                break;
            case A_LSR_T:
                op2 = R64(instr.rm) >> instr.shift;
            case A_ASR_T:
                // WONT-FIX: non portable code
                op2 = (int64_t)R64(instr.rm) >> instr.shift;
                break;
            default:
                fprintf(stderr,
                        "Unknown shift type: 0x%x for data processing (Register) arithmetic instruction\n",
                        dpr.type);
                exit(EXIT_FAILURE);
                break;
            }
            execute_arit_instr(reg, instr.atype, instr.sf, instr.rd, instr.rn, op2);
        } else {
            // 32 bit mode
            uint32_t op2;
            switch (instr.stype) {
            case A_LSL_T:
                op2 = R32(instr.rm) << instr.shift;
                break;
            case A_LSR_T:
                op2 = R32(instr.rm) >> instr.shift;
            case A_ASR_T:
                // WONT-FIX: non portable code
                op2 = (int32_t)R32(instr.rm) >> instr.shift;
                break;
            default:
                fprintf(stderr,
                        "Unknown shift type: 0x%x for data processing (Register) arithmetic instruction\n",
                        dpr.type);
                exit(EXIT_FAILURE);
                break;
            }
            execute_arit_instr(reg, instr.atype, instr.sf, instr.rd, instr.rn, (uint64_t)op2);
        }
        break;
    }
    case BIT_LOGIC_T: {
        Logical instr = dpr.logical;
        uint32_t op2;
        if (instr.sf) {
            // 64 bit mode
            uint64_t rm = R64(instr.rm);
            uint64_t rn = R64(instr.rn);
            switch (instr.stype) {
            case L_LSL_T:
                op2 = rm << instr.shift;
            case L_LSR_T:
                op2 = rm >> instr.shift;
            case L_ASR_T:
                // WONT-FIX: non portable code
                op2 = (int64_t)rm >> instr.shift;
            case L_ROR_T:
                // https://stackoverflow.com/questions/28303232/rotate-right-using-bit-operation-in-c
                op2 = (rm >> instr.shift) | (rm << (64 - instr.shift));
            default:
                fprintf(stderr,
                        "Unknown shift type: 0x%x for data processing (Register) bit logic instruction\n",
                        instr.stype);
                exit(EXIT_FAILURE);
            }
            if (instr.N) {
                op2 = ~op2;
            }
            switch (instr.btype) {
            case AND:
                R64(instr.rd) = rn & op2;
                break;
            case OR:
                R64(instr.rd) = rn | op2;
                break;
            case EO:
                R64(instr.rd) = rn ^ op2;
                break;
            case ANDC:
                R64(instr.rd) = rn & op2;
                reg->PSTATE->N = sgn64(R64(instr.rd));
                reg->PSTATE->Z = R64(instr.rd) == 0;
                reg->PSTATE->C = 0;
                reg->PSTATE->V = 0;
                break;
            default:
                fprintf(stderr,
                        "Unknown bit logic type: 0x%x for data processing (Register) bit logic instruction\n",
                        instr.btype);
                exit(EXIT_FAILURE);
            }
        } else {
            // 32 bit mode
            uint32_t rm = R32(instr.rm);
            uint32_t rn = R32(instr.rn);
            switch (instr.stype) {
            case L_LSL_T:
                op2 = rm << instr.shift;
            case L_LSR_T:
                op2 = rm >> instr.shift;
            case L_ASR_T:
                // WONT-FIX: non portable code
                op2 = (int32_t)rm >> instr.shift;
            case L_ROR_T:
                // https://stackoverflow.com/questions/28303232/rotate-right-using-bit-operation-in-c
                op2 = (rm >> instr.shift) | (rm << (32 - instr.shift));
            default:
                fprintf(stderr,
                        "Unknown shift type: 0x%x for data processing (Register) bit logic instruction\n",
                        instr.stype);
                exit(EXIT_FAILURE);
            }
            if (instr.N) {
                op2 = ~op2;
            }
            switch (instr.btype) {
            case AND:
                R32(instr.rd) = rn & op2;
                break;
            case OR:
                R32(instr.rd) = rn | op2;
                break;
            case EO:
                R32(instr.rd) = rn ^ op2;
                break;
            case ANDC:
                R32(instr.rd) = rn & op2;
                reg->PSTATE->N = sgn32(R32(instr.rd));
                reg->PSTATE->Z = R32(instr.rd) == 0;
                reg->PSTATE->C = 0;
                reg->PSTATE->V = 0;
                break;
            default:
                fprintf(stderr,
                        "Unknown bit logic type: 0x%x for data processing (Register) bit logic instruction\n",
                        instr.btype);
                exit(EXIT_FAILURE);
            }
        }
        break;
    }
    case MULTIPLY_T: {
        Multiply instr = dpr.multiply;
        if (instr.sf) {
            // 64 bit mode
            uint64_t ra;
            if (instr.ra == MULT_ZERO_REG) {
                ra = reg->ZR;
            } else {
                ra = R64(instr.ra);
            }
            if (instr.x) {
                // negate result, aka msub
                R64(instr.rd) = ra - (R64(instr.rn) * R64(instr.rm));
            } else {
                // don't negate result, aka madd
                R64(instr.rd) = ra + (R64(instr.rn) * R64(instr.rm));
            }
        } else {
            // 32 bit mode
            uint32_t ra;
            if (instr.ra == MULT_ZERO_REG) {
                ra = reg->ZR;
            } else {
                ra = R32(instr.ra);
            }
            if (instr.x) {
                // negate result, aka msub
                R32(instr.rd) = ra - (R32(instr.rn) * R32(instr.rm));
            } else {
                // don't negate result, aka madd
                R32(instr.rd) = ra + (R32(instr.rn) * R32(instr.rm));
            }
        }
        break;
    }
    default:
        fprintf(stderr, "Unknown data processing (Register) type: 0x%x\n", dpr.type);
        exit(EXIT_FAILURE);
        break;
    }
}
void execute_sdt(Register *reg, SdTrans sdt) {
    uint64_t addrs;
    switch (sdt.type) {
    case SD_REGISTER_T: {
        SDRegister instr = sdt.reg;
        addrs = R64(instr.xn) + R64(instr.xm);
        if (instr.L) {
            // load operation
            if (instr.sf) {
                // 64 bits
                R64(instr.rt) = * (reg->ram + addrs);
            } else {
                // 32 bits
                R32(instr.rt) = * (uint32_t *) (reg->ram + addrs);
                R32_cls_upper(instr.rt);
            }
        } else {
            // store operation
            if (instr.sf) {
                // 64 bits
                * (reg->ram + addrs) = R64(instr.rt);
            } else {
                // 32 bits
                * (uint32_t *) (reg->ram + addrs) = R32(instr.rt);
            }
        }
    }
    case PRE_POST_INDEX_T: {
        PrePostIndex instr = sdt.pre_post_index;
        switch (instr.itype) {
        case PRE_INDEX: {
            addrs = R64(instr.xn) + (uint64_t) instr.simm9;
            // transfer address
            if (instr.L) {
                // load operation
                if (instr.sf) {
                    // 64 bits
                    R64(instr.rt) = * (reg->ram + addrs);
                } else {
                    // 32 bits
                    R32(instr.rt) = * (uint32_t *) (reg->ram + addrs);
                    R32_cls_upper(instr.rt);
                }
            } else {
                // store operation
                if (instr.sf) {
                    // 64 bits
                    * (reg->ram + addrs) = R64(instr.rt);
                } else {
                    // 32 bits
                    * (uint32_t *) (reg->ram + addrs) = R32(instr.rt);
                }
            }
            R64(instr.xn) = R64(instr.xn) + (uint64_t) instr.simm9;
            break;
        }

        case POST_INDEX: {
            addrs = R64(instr.xn);
            if (instr.L) {
                // load operation
                if (instr.sf) {
                    // 64 bits
                    R64(instr.rt) = * (reg->ram + addrs);
                } else {
                    // 32 bits
                    R32(instr.rt) = * (uint32_t *) (reg->ram + addrs);
                    R32_cls_upper(instr.rt);
                }
            } else {
                // store operation
                if (instr.sf) {
                    // 64 bits
                    * (reg->ram + addrs) = R64(instr.rt);
                } else {
                    // 32 bits
                    * (uint32_t *) (reg->ram + addrs) = R32(instr.rt);
                }
            }
            R64(instr.xn) = R64(instr.xn) + (uint64_t) instr.simm9;
            break;
        }

        default:
            fprintf(stderr, "Unknown Index type: 0x%x\n", instr.itype);
            exit(EXIT_FAILURE);
        }
        break;
    }
    case UNSIGN_T: {
        Unsigned instr = sdt.usigned;
        if (instr.L) {
            // load operation
            if (instr.sf) {
                // 64 bits
                addrs = R64(instr.xn) + (uint64_t) (instr.imm12 << 3);
                R64(instr.rt) = * (reg->ram + addrs);
            } else {
                // 32 bits
                addrs = R64(instr.xn) + (uint64_t) (instr.imm12 << 2);
                R32(instr.rt) = * (uint32_t *) (reg->ram + addrs);
                R32_cls_upper(instr.rt);
            }
        } else {
            // store operation
            if (instr.sf) {
                // 64 bits
                addrs = R64(instr.xn) + (uint64_t) (instr.imm12 << 3);
                * (reg->ram + addrs) = R64(instr.rt);
            } else {
                // 32 bits
                addrs = R64(instr.xn) + (uint64_t) (instr.imm12 << 2);
                * (uint32_t *) (reg->ram + addrs) = R32(instr.rt);
            }
        }
        break;
    }
    default:
        fprintf(stderr, "Unknown Single Data Transfer type: 0x%x\n", sdt.type);
        exit(EXIT_FAILURE);
    }
}
void execute_ldl(Register *reg, LoadLiteral ldl) {
    uint64_t addrs = reg->PC + (uint64_t) (ldl.simm19 * 4);
    if (ldl.sf) {
        // 64 bits
        R64(ldl.rt) = * (reg->ram + addrs);
    } else {
        // 32 bits
        R32(ldl.rt) = * (uint32_t *) (reg->ram + addrs);
        R32_cls_upper(ldl.rt);
    }
}
void execute_branch(Register *reg, Branch branch) {
    // TODO: ky723
}
