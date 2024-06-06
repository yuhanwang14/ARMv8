#include "dp_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t OPC_ADD = 0;
static const uint8_t OPC_ADDS = 1;
static const uint8_t OPC_AND = 0;
static const uint8_t OPC_ANDS = 3;
static const uint8_t OPC_BIC = 0;
static const uint8_t OPC_BICS = 3;
static const uint8_t OPC_EOR = 2;
static const uint8_t OPC_EON = 2;
static const uint8_t OPC_ORR = 1;
static const uint8_t OPC_ORN = 1;
static const uint8_t OPC_SUB = 2;
static const uint8_t OPC_SUBS = 3;
static const uint8_t OPC_MOVN = 0;
static const uint8_t OPC_MOVZ = 2;
static const uint8_t OPC_MOVK = 3;
static const uint8_t OPC_SIZE = 2;
static const uint8_t DPI_WIDE_MOVE_OPI = 5;
static const uint8_t DPI_ARITH_OPI = 2;
static const uint8_t DPI_IMM_26_28 = 4;
static const uint8_t DPI_REG_ARITH_LOGIC_25_28 = 5;
static const uint16_t DPI_MUL_21_30 = 216;

uint32_t parse_2op_with_dest(char *opcode, char **arguments) {
    int8_t opc;        // holds the opc for this instruction
    int8_t negate = 0; // holds the value for N
    bool logicOp = false;
    uint32_t result = 0;
    if (STR_EQ(opcode, "add")) {
        opc = OPC_ADD;
    } else if (STR_EQ(opcode, "adds")) {
        opc = OPC_ADDS;
    } else if (STR_EQ(opcode, "sub")) {
        opc = OPC_SUB;
    } else if (STR_EQ(opcode, "subs")) {
        opc = OPC_SUBS;
    } else if (STR_EQ(opcode, "and")) {
        opc = OPC_AND;
        logicOp = true;
    } else if (STR_EQ(opcode, "ands")) {
        opc = OPC_ANDS;
        logicOp = true;
    } else if (STR_EQ(opcode, "bic")) {
        opc = OPC_BIC;
        logicOp = true;
        negate = 1;
    } else if (STR_EQ(opcode, "bics")) {
        opc = OPC_BICS;
        logicOp = true;
        negate = 1;
    } else if (STR_EQ(opcode, "eor")) {
        opc = OPC_EOR;
        logicOp = true;
    } else if (STR_EQ(opcode, "eon")) {
        opc = OPC_EON;
        logicOp = true;
        negate = 1;
    } else if (STR_EQ(opcode, "orr")) {
        opc = OPC_ORR;
        logicOp = true;
    } else if (STR_EQ(opcode, "orn")) {
        opc = OPC_ORN;
        logicOp = true;
        negate = 1;
    } else {
        fprintf(stderr, "failed to parse opcode for\n '%s'\nas 2 operand with des\n", opcode);
        exit(EXIT_FAILURE);
    }
    bit_append(&result, GET_SF(arguments[0]), SF_SIZE); // sf, pos 31
    bit_append(&result, opc, OPC_SIZE);                  // opc,pos 29 - 30
    if (is_literal(arguments[2])) {
        // <operand> is a literal value
        // parsed as a DPI immediate
        bit_append(&result, DPI_IMM_26_28, 3); // pos 26 - 28
        bit_append(&result, DPI_ARITH_OPI, 3); // opi, pos 23 - 25
        bit_append(&result, parse_imm12(arguments[2], arguments[3], arguments[4]),
                   13); // sh & imm12, pos 10 - 22
        bit_append(&result, parse_register(arguments[1]), REGISTER_ADR_SIZE); // rn, pos 5 - 9
        bit_append(&result, parse_register(arguments[0]), REGISTER_ADR_SIZE); // rd, pos 0 - 4
    } else {
        // <operand> is not a literal value
        // parsed as DPI register
        bit_append(&result, DPI_REG_ARITH_LOGIC_25_28, 4); // pos 25 - 28
        if (logicOp) {
            bit_append(&result, 0, 1); // pos 24 for bit logic
        } else {
            bit_append(&result, 1, 1); // pos 24 for arith
        }
        uint8_t *parsedShift = parse_shift(arguments[3], arguments[4]);
        bit_append(&result, parsedShift[0], 2); // shift in opr, pos 22 - 23
        bit_append(&result, negate, 1);         // the N value, set to zero for arthi ops, pos 21
        bit_append(&result, parse_register(arguments[2]), REGISTER_ADR_SIZE); // rm, pos 16 - 20
        bit_append(&result, parsedShift[1], 6); // operand for shift value, pos 10-15
        bit_append(&result, parse_register(arguments[1]), REGISTER_ADR_SIZE); // rn, pos 5 - 9
        bit_append(&result, parse_register(arguments[0]), REGISTER_ADR_SIZE); // rd, pos 0 - 4
        free(parsedShift);
    }
    return result;
}

uint32_t parse_multiply(char *opcode, char **arguments) {
    uint32_t result = 0;
    bit_append(&result, GET_SF(arguments[0]), SF_SIZE);                         // sf, pos 31
    bit_append(&result, DPI_MUL_21_30, 10);                               // pos 21 - 30
    bit_append(&result, parse_register(arguments[2]), REGISTER_ADR_SIZE); // rm, pos 16 - 20
    if (STR_EQ(opcode, "madd")) {
        bit_append(&result, 0, 1); // x, holds whether a negation is needed, pos 15
    } else if (STR_EQ(opcode, "msub")) {
        bit_append(&result, 1, 1); // x, holds whether a negation is needed, pos 15
    } else {
        fprintf(stderr, "failed to parse opcode for\n'%s'\nas multiply\n", opcode);
        exit(EXIT_FAILURE);
    }
    bit_append(&result, parse_register(arguments[3]), REGISTER_ADR_SIZE); // ra, pos 10 - 14
    bit_append(&result, parse_register(arguments[1]), REGISTER_ADR_SIZE); // rn, pos 5 - 9
    bit_append(&result, parse_register(arguments[0]), REGISTER_ADR_SIZE); // rd, pos 0 - 4
    return result;
}

uint32_t parse_wide_move(char *opcode, char **arguments) {
    uint32_t result = 0;
    bit_append(&result, GET_SF(arguments[0]), SF_SIZE);
    if (STR_EQ(opcode, "movn")) {
        bit_append(&result, OPC_MOVN, OPC_SIZE); // opc, pos 29 - 30
    } else if (STR_EQ(opcode, "movz")) {
        bit_append(&result, OPC_MOVZ, OPC_SIZE);
    } else if (STR_EQ(opcode, "movk")) {
        bit_append(&result, OPC_MOVK, OPC_SIZE);
    } else {
        fprintf(stderr, "failed to parse opcode for\n'%s'\nas wide move\n", opcode);
        exit(EXIT_FAILURE);
    }
    bit_append(&result, DPI_IMM_26_28, 3);     // (immediate dp) pos 26 - 28
    bit_append(&result, DPI_WIDE_MOVE_OPI, 3); // opi, pos 23 - 25
    bit_append(&result, parse_imm16(arguments[1], arguments[2], arguments[3]),
               18); // operand as hw and imm 16, pos 5 - 22
    bit_append(&result, parse_register(arguments[0]), REGISTER_ADR_SIZE); // rd, pos 0 - 4
    return result;
}
