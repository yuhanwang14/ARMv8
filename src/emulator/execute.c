#include "execute.h"
#include "utils.h"
#include <features.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// register related macros
#define R64(n) reg->g_reg[n]
#define R32(n) ((uint32_t *)reg->g_reg)[(n)*2]
#define R32_cls_upper(n) ((uint32_t *)reg->g_reg)[(n)*2 + 1] = 0
// getting sign bit macro
#define SGN64(n) (bool)((n >> 63) & 1)
#define SGN32(n) (bool)((n >> 31) & 1)
// getting 16 bit long slices of register
#define R64_16(n, shift) ((uint16_t *)reg->g_reg)[(n)*4 + shift]
#define R32_16(n, shift) R64_16(n, shift)
// get ram with offset, ram is always BYTE INDEXED
#define RAM_64(n) *(uint64_t *)(reg->ram + (n))
#define RAM_32(n) *(uint32_t *)(reg->ram + (n))
// offset the PC if condition is statisfied
#define JUMP_IF(cond) reg->PC = (cond) ? reg->PC + offset : reg->PC

static const uint32_t MULT_ZERO_REG = 0x1F;

// prints debug message and quit
static void fail(const char *message, unsigned opt) {
    fprintf(stderr, "Unknown arithmetic type: 0x%x\n", opt);
    exit(EXIT_FAILURE);
}

static void load(Register *reg, bool sf, uint64_t rt, uint64_t addrs) {
    // load operation
    if (sf) {
        // 64 bits
        R64(rt) = RAM_64(addrs);
    } else {
        // 32 bits
        R32(rt) = RAM_32(addrs);
        R32_cls_upper(rt);
    }
}

static void store(Register *reg, bool sf, uint64_t rt, uint64_t addrs) {
    // store operation
    if (sf) {
        // 64 bits
        RAM_64(addrs) = R64(rt);
    } else {
        // 32 bits
        RAM_32(addrs) = R32(rt);
    }
}

// generalisation of arithmetic operations
static void execute_arit_instr(Register *reg, ArithmeticType atype, bool sf, uint32_t rd,
                               uint32_t rn, uint64_t op2) {
    uint32_t op2_32 = (uint32_t)op2;
    switch (atype) {
    case ADD:
        // addition
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
        // addition with state update
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
            reg->PSTATE->N = SGN64(i_result);
            reg->PSTATE->Z = u_result == 0;
        } else {
            // 32 bit mode
            uint32_t u_result;
            int32_t i_result;
            reg->PSTATE->C = __builtin_add_overflow(R32(rn), op2_32, &u_result);
            reg->PSTATE->V = __builtin_add_overflow((int32_t)R32(rn), (int32_t)op2_32, &i_result);
            R32_cls_upper(rd);
            R64(rd) = u_result;
            reg->PSTATE->N = SGN32(i_result);
            reg->PSTATE->Z = u_result == 0;
        }
        break;
    case SUB:
        // subtraction
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
        // subtraction with state update
        if (sf) {
            // 64 bit mode
            uint64_t u_result;
            int64_t i_result;
            // ironically the function is likely implemented using these instructions
            // update unsigned overflow indicator flags
            reg->PSTATE->C = !__builtin_sub_overflow(R64(rn), op2, &u_result);
            // update signed overflow indicator flags
            reg->PSTATE->V = __builtin_sub_overflow((int64_t)R64(rn), (int64_t)op2, &i_result);
            R64(rd) = u_result;
            reg->PSTATE->N = SGN64(i_result);
            reg->PSTATE->Z = u_result == 0;
        } else {
            // 32 bit mode
            uint32_t u_result;
            int32_t i_result;
            reg->PSTATE->C = !__builtin_sub_overflow(R32(rn), op2_32, &u_result);
            reg->PSTATE->V = __builtin_sub_overflow((int32_t)R32(rn), (int32_t)op2_32, &i_result);
            R32_cls_upper(rd);
            R64(rd) = u_result;
            reg->PSTATE->N = SGN32(i_result);
            reg->PSTATE->Z = u_result == 0;
        }
        break;
    default:
        fail("Unknown arithmetic type: 0x%x\n", (unsigned)atype);
    }
}

// Executes a DP (Immediate) instruction
static void execute_dpi(Register *reg, DpImmed dpi) {
    switch (dpi.type) {
    case DPI_ARITHMETIC_T: {
        // arithmetic operation
        DPIArithmetic instr = dpi.arithmetic;
        uint32_t op2 = instr.sh ? instr.imm12 << 12 : instr.imm12;
        execute_arit_instr(reg, instr.atype, instr.sf, instr.rd, instr.rn, (uint64_t)op2);
        break;
    }
    case WIDE_MOVE_T: {
        // wide move operation
        WideMove instr = dpi.wide_move;
        uint64_t op = (uint64_t)instr.imm16 << (instr.hw * 16);
        switch (instr.mtype) {
        case MOVN: {
            // move with negate
            if (instr.sf) {
                // 64 bit mode
                R64(instr.rd) = ~op;
            } else {
                // 32 bit mode
                R32(instr.rd) = ~(uint32_t)op;
                R32_cls_upper(instr.rd);
            }
            break;
        }
        case MOVZ: {
            // move with zero
            if (instr.sf) {
                // 64 bit mode
                R64(instr.rd) = op;
            } else {
                // 32 bit mode
                R32(instr.rd) = (uint32_t)op;
                R32_cls_upper(instr.rd);
            }
            break;
        }
        case MOVK: {
            // move with keep
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
            fail("Unknown wide move type: 0x%x\n", (unsigned)instr.mtype);
        }
        break;
    }
    default:
        fail("Unknown data processing (Immediate) type: 0x%x\n", (unsigned)dpi.type);
    }
}

// Executes a DP (Register) instruction
static void execute_dpr(Register *reg, DpRegister dpr) {
    switch (dpr.type) {
    case DPR_ARITHMETIC_T: {
        // arithmetic operation
        DPRArithmetic instr = dpr.arithmetic;
        if (instr.sf) {
            // 64 bit mode
            uint64_t op2;
            switch (instr.stype) {
            case A_LSL_T:
                // left shift
                op2 = R64(instr.rm) << instr.shift;
                break;
            case A_LSR_T:
                // right shift
                op2 = R64(instr.rm) >> instr.shift;
                break;
            case A_ASR_T:
                // right arithmetic shift
                // WONT-FIX: non portable code
                op2 = (int64_t)R64(instr.rm) >> instr.shift;
                break;
            default:
                fail("Unknown shift type: 0x%x for data processing (Register) arithmetic "
                     "instruction\n",
                     (unsigned)dpr.type);
            }
            execute_arit_instr(reg, instr.atype, instr.sf, instr.rd, instr.rn, op2);
        } else {
            // 32 bit mode
            uint32_t op2;
            switch (instr.stype) {
            case A_LSL_T:
                // left shift
                op2 = R32(instr.rm) << instr.shift;
                break;
            case A_LSR_T:
                // right shift
                op2 = R32(instr.rm) >> instr.shift;
                break;
            case A_ASR_T:
                // right arithmetic shift
                // WONT-FIX: non portable code
                op2 = (int32_t)R32(instr.rm) >> instr.shift;
                break;
            default:
                fail("Unknown shift type: 0x%x for data processing (Register) arithmetic "
                     "instruction\n",
                     (unsigned)dpr.type);
            }
            execute_arit_instr(reg, instr.atype, instr.sf, instr.rd, instr.rn, (uint64_t)op2);
        }
        break;
    }
    case BIT_LOGIC_T: {
        Logical instr = dpr.logical;
        if (instr.sf) {
            // 64 bit mode
            uint64_t op2;
            uint64_t rm = R64(instr.rm);
            uint64_t rn = R64(instr.rn);
            switch (instr.stype) {
            case L_LSL_T:
                // left shift
                op2 = rm << instr.shift;
                break;
            case L_LSR_T:
                // right shift
                op2 = rm >> instr.shift;
                break;
            case L_ASR_T:
                // left arithmetic shift
                // WONT-FIX: non portable code
                op2 = (int64_t)rm >> instr.shift;
                break;
            case L_ROR_T:
                // rotate right
                // https://stackoverflow.com/questions/28303232/rotate-right-using-bit-operation-in-c
                op2 = (rm >> instr.shift) | (rm << (64 - instr.shift));
                break;
            default:
                fail("Unknown shift type: 0x%x for data processing (Register) bit logic "
                     "instruction\n",
                     (unsigned)instr.stype);
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
                reg->PSTATE->N = SGN64(R64(instr.rd));
                reg->PSTATE->Z = R64(instr.rd) == 0;
                reg->PSTATE->C = 0;
                reg->PSTATE->V = 0;
                break;
            default:
                fail("Unknown bit logic type: 0x%x for data processing (Register) bit logic "
                     "instruction\n",
                     (unsigned)instr.btype);
            }
        } else {
            // 32 bit mode
            uint32_t op2;
            uint32_t rm = R32(instr.rm);
            uint32_t rn = R32(instr.rn);
            switch (instr.stype) {
            case L_LSL_T:
                // left shift
                op2 = rm << instr.shift;
                break;
            case L_LSR_T:
                // right shift
                op2 = rm >> instr.shift;
                break;
            case L_ASR_T:
                // arithmetic shift
                // WONT-FIX: non portable code
                op2 = (int32_t)rm >> instr.shift;
                break;
            case L_ROR_T:
                // rotate right
                // https://stackoverflow.com/questions/28303232/rotate-right-using-bit-operation-in-c
                op2 = (rm >> instr.shift) | (rm << (32 - instr.shift));
                break;
            default:
                fail("Unknown shift type: 0x%x for data processing (Register) bit logic "
                     "instruction\n",
                     (unsigned)instr.stype);
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
                reg->PSTATE->N = SGN32(R32(instr.rd));
                reg->PSTATE->Z = R32(instr.rd) == 0;
                reg->PSTATE->C = 0;
                reg->PSTATE->V = 0;
                break;
            default:
                fail("Unknown bit logic type: 0x%x for data processing (Register) bit logic "
                     "instruction\n",
                     (unsigned)instr.btype);
            }
            R32_cls_upper(instr.rd);
        }
        break;
    }
    case MULTIPLY_T: {
        Multiply instr = dpr.multiply;
        if (instr.sf) {
            // 64 bit mode
            uint64_t ra;
            // check if it's zero vector
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
            // check if it's zero vector
            if (instr.ra == MULT_ZERO_REG) {
                ra = reg->ZR;
            } else {
                ra = R32(instr.ra);
            }
            if (instr.x) {
                // negate result, aka msub
                R32(instr.rd) = ra - (R32(instr.rn) * R32(instr.rm));
                R32_cls_upper(instr.rd);
            } else {
                // don't negate result, aka madd
                R32(instr.rd) = ra + (R32(instr.rn) * R32(instr.rm));
                R32_cls_upper(instr.rd);
            }
        }
        break;
    }
    default:
        fail("Unknown data processing (Register) type: 0x%x\n", (unsigned)dpr.type);
    }
}

// Execute single data transfer instruction
static void execute_sdt(Register *reg, SdTrans sdt) {
    uint64_t addrs;
    switch (sdt.type) {
    case SD_REGISTER_T: {
        SDRegister instr = sdt.reg;
        addrs = R64(instr.xn) + R64(instr.xm);
        if (instr.L) {
            // load operation
            load(reg, instr.sf, instr.rt, addrs);
        } else {
            // store operation
            store(reg, instr.sf, instr.rt, addrs);
        }
        break;
    }
    case PRE_POST_INDEX_T: {
        PrePostIndex instr = sdt.pre_post_index;
        switch (instr.itype) {
        case PRE_INDEX: {
            addrs = R64(instr.xn) + (uint64_t)(instr.simm9);
            R64(instr.xn) = R64(instr.xn) + (uint64_t)(instr.simm9);
            // transfer address
            if (instr.L) {
                // load operation
                load(reg, instr.sf, instr.rt, addrs);
            } else {
                // store operation
                store(reg, instr.sf, instr.rt, addrs);
            }
            break;
        }

        case POST_INDEX: {
            addrs = R64(instr.xn);
            R64(instr.xn) = R64(instr.xn) + (uint64_t)(instr.simm9);
            if (instr.L) {
                // load operation
                load(reg, instr.sf, instr.rt, addrs);
            } else {
                // store operation
                store(reg, instr.sf, instr.rt, addrs);
            }
            break;
        }

        default:
            fail("Unknown Index type: 0x%x\n", (unsigned)instr.itype);
        }
        break;
    }
    case UNSIGN_T: {
        Unsigned instr = sdt.usigned;
        addrs = instr.sf ? R64(instr.xn) + (uint64_t)(instr.imm12 * 8)
                         : R64(instr.xn) + (uint64_t)(instr.imm12 * 4);
        if (instr.L) {
            // load operation
            load(reg, instr.sf, instr.rt, addrs);
        } else {
            // store operation
            store(reg, instr.sf, instr.rt, addrs);
        }
        break;
    }
    default:
        fail("Unknown Single Data Transfer type: 0x%x\n", (unsigned)sdt.type);
    }
}

// Execute a load literal instruction
static void execute_ldl(Register *reg, LoadLiteral ldl) {
    uint64_t addrs = reg->PC + (uint64_t)(ldl.simm19) * sizeof(uint32_t);
    load(reg, ldl.sf, ldl.rt, addrs);
}

// execute a branch instruction
static void execute_branch(Register *reg, Branch branch) {
    switch (branch.type) {
    case UNCONDITIONAL_T: {
        Unconditional instr = branch.unconditional;
        reg->PC += instr.offset * sizeof(uint32_t);
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
        // offset is byte indexed instead of word indexed
        int64_t offset = instr.offset;
        switch (instr.cond) {
        case EQ:
            JUMP_IF(ps.Z);
            break;
        case NE:
            JUMP_IF(!ps.Z);
            break;
        case GE:
            JUMP_IF(ps.N == ps.V);
            break;
        case LT:
            JUMP_IF(ps.N != ps.V);
            break;
        case GT:
            JUMP_IF(!ps.Z && ps.N == ps.V);
            break;
        case LE:
            JUMP_IF(!(!ps.Z && ps.N == ps.V));
            break;
        case AL:
            JUMP_IF(true);
            break;
        default:
            fail("Unknown branch conditional type: 0x%x\n", (unsigned)instr.cond);
        }
        break;
    }
    default:
        fail("Unknown branch type: 0x%x\n", (unsigned)branch.type);
    }
}

void execute(Register *reg, Instr *instr) {
    uint64_t PC_prev = reg->PC;
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
        fail("Unknown instruction type: 0x%x\n", (unsigned)instr->type);
    }
    // update program counter if it was not changed by branch operation
    if (reg->PC == PC_prev) {
        reg->PC += 4;
    }
    // reset the zero register to zero, in case we wrote to it
    reg->ZR = 0;
}
