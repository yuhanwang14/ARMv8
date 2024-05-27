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
void execute_sdt(Register *reg, SdTrans sdt) {
    uint32_t addrs;
    switch (sdt.type) {
    case SD_REGISTER_T: {
        SDRegister instr = sdt.reg;
        addrs = R64(instr.xn) + R64(instr.xm);                   // transfer address
        if (instr.L) {                                           // load operation
            if (instr.sf) {                                      // 64 bits
                R64(instr.rt) = *(uint64_t *)(reg->ram + addrs); // may have to fix this part. TODO...
            } else {                                             // 32 bits
                R32(instr.rt) = *(reg->ram + addrs);
                R32_cls_upper(instr.rt);
            }
        } else {                                                 // store operation
            if (instr.sf) {
                *(uint64_t *)(reg->ram + addrs) = R64(instr.rt);
            } else {
                *(reg->ram + addrs) = R32(instr.rt);
                R32_cls_upper(instr.rt);
            }
        }
    }
    case PRE_POST_INDEX_T: {
        PrePostIndex instr = sdt.pre_post_index;
        switch (instr.itype) {
        case PRE_INDEX: {
            addrs = R64(instr.xn) + instr.simm9;                     // transfer address
            if (instr.L) {                                           // load operation
                if (instr.sf) {                                      // 64 bits
                    R64(instr.rt) = *(uint64_t *)(reg->ram + addrs); // may have to fix this part. TODO...
                } else {                                             // 32 bits
                    R32(instr.rt) = *(reg->ram + addrs);
                    R32_cls_upper(instr.rt);
                }
            } else {                                                 // store operation
                if (instr.sf) {
                    *(uint64_t *)(reg->ram + addrs) = R64(instr.rt);
                } else {
                    *(reg->ram + addrs) = R32(instr.rt);
                    R32_cls_upper(instr.rt);
                }
            }
            R64(instr.xn) = R64(instr.xn) + instr.simm9;
            break;
        }

        case POST_INDEX: {
            addrs = R64(instr.xn);
            if (instr.L) {                                           // load operation
                if (instr.sf) {                                      // 64 bits
                    R64(instr.rt) = *(uint64_t *)(reg->ram + addrs); // may have to fix this part. TODO...
                } else {                                             // 32 bits
                    R32(instr.rt) = *(reg->ram + addrs);
                    R32_cls_upper(instr.rt);
                }
            } else {                                                 // store operation
                if (instr.sf) {
                    *(uint64_t *)(reg->ram + addrs) = R64(instr.rt);
                } else {
                    *(reg->ram + addrs) = R32(instr.rt);
                    R32_cls_upper(instr.rt);
                }
            }
            R64(instr.xn) = R64(instr.xn) + instr.simm9;
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
            if (instr.sf) {
                addrs = R64(instr.xn) + (instr.imm12 << 3);                                   
                R64(instr.rt) = *(uint64_t *)(reg->ram + addrs); 
            } else {                                 
                addrs = R64(instr.xn) + (instr.imm12 << 2);           
                R32(instr.rt) = *(reg->ram + addrs);
                R32_cls_upper(instr.rt);
            }
        } else {                                           
            if (instr.sf) {
                addrs = R64(instr.xn) + (instr.imm12 << 3);
                *(uint64_t *)(reg->ram + addrs) = R64(instr.rt);
            } else {
                addrs = R64(instr.xn) + (instr.imm12 << 2);
                *(reg->ram + addrs) = R32(instr.rt);
                R32_cls_upper(instr.rt);
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
    uint32_t addrs = reg->PC + (ldl.simm19 << 2);                              
    if (ldl.sf) {                                
        R64(ldl.rt) = *(uint64_t *)(reg->ram + addrs); 
    } else {                                         
        R32(ldl.rt) = *(reg->ram + addrs);
        R32_cls_upper(ldl.rt);
    }

}
void execute_branch(Register *reg, Branch branch) {
    // TODO: ky723
}
