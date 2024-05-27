#include "dp_parser.h"

const uint8_t OPC_ADD = 0;
const uint8_t OPC_ADDS = 1;
const uint8_t OPC_AND = 0;
const uint8_t OPC_ANDS = 3;
const uint8_t OPC_BIC = 0;
const uint8_t OPC_BICS = 3;
const uint8_t OPC_EOR = 2;
const uint8_t OPC_EON = 2;
const uint8_t OPC_ORR = 1;
const uint8_t OPC_ORN = 1;
const uint8_t OPC_SUB = 2;
const uint8_t OPC_SUBS = 3;
const uint8_t DPI_IMM_ARITHMETIC_23_28 = 33;
const uint8_t DPI_REG_ARITH_LOGIC_25_28 = 5;


uint32_t parse_two_operands_with_des(char *instruction) {
    char **buffer = NULL;
    char opcode[5];
    strcpy(opcode, strtok_r(instruction, " ", buffer));
    char *arguments[4];
    int8_t opc;
    int8_t negate = 0; // holds the value for N
    bool logicOp = false;
    uint32_t result = 0;
    for (int i = 0; i < 4; i++) {
        arguments[i] = strtok_r(NULL, " ", buffer);
    }
    if (strcmp(opcode,"add") == 0){
        opc = OPC_ADD;
    }else if (strcmp(opcode,"adds") == 0){
        opc = OPC_ADDS;
    }else if (strcmp(opcode,"sub") == 0){
        opc = OPC_SUB;
    }else if (strcmp(opcode,"subs") == 0){
        opc = OPC_SUBS;
    }else if (strcmp(opcode,"and") == 0){
        opc = OPC_AND;
        logicOp = true;
    }else if (strcmp(opcode,"ands") == 0){
        opc = OPC_ANDS;
        logicOp = true;
    }else if (strcmp(opcode,"bic") == 0){
        opc = OPC_BIC;
        logicOp = true;
        negate = 1;
    }else if (strcmp(opcode,"bics") == 0){
        opc = OPC_BICS;
        logicOp = true;
        negate = 1;
    }else if (strcmp(opcode,"eor") == 0){
        opc = OPC_EOR;
        logicOp = true;
    }else if (strcmp(opcode,"eon") == 0){
        opc = OPC_EON;
        logicOp = true;
        negate = 1;
    }else if (strcmp(opcode,"orr") == 0){
        opc = OPC_ORR;
        logicOp = true;
    }else if (strcmp(opcode,"orn") == 0){
        opc = OPC_ORN;
        logicOp = true;
        negate = 1;
    }else{
        fprintf(stderr,"failed to parse opcode for\n '%s'",instruction);
        exit(EXIT_FAILURE);
    }
    appendBits(&result,(arguments[0][0] == 'x'),1);
    appendBits(&result,opc,2);
    if (is_literal(arguments[2])) {
        appendBits(&result,DPI_IMM_ARITHMETIC_23_28,6); //23
        appendBits(&result, parse_imm12(arguments[2],arguments[3]),13);
        appendBits(&result, parse_register(arguments[1]),REGISTER_ADR_SIZE);
        appendBits(&result, parse_register(arguments[0]),REGISTER_ADR_SIZE);
    } else {
        appendBits(&result,DPI_REG_ARITH_LOGIC_25_28,4);
        if (logicOp) {
            appendBits(&result,0,1);
        } else {
            appendBits(&result,1,1);
        }
        uint8_t *parsedShift = parse_shift(arguments[3]);
        appendBits(&result,parsedShift[0],2);
        appendBits(&result,negate,1);
        appendBits(&result, parse_register(arguments[2]),REGISTER_ADR_SIZE);
        appendBits(&result,parsedShift[1],6);
        appendBits(&result, parse_register(arguments[1]),REGISTER_ADR_SIZE);
        appendBits(&result, parse_register(arguments[0]),REGISTER_ADR_SIZE);
    }
    return result;
}