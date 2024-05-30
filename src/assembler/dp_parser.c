//#include "dp_parser.h"
//
//const uint8_t OPC_ADD = 0;
//const uint8_t OPC_ADDS = 1;
//const uint8_t OPC_AND = 0;
//const uint8_t OPC_ANDS = 3;
//const uint8_t OPC_BIC = 0;
//const uint8_t OPC_BICS = 3;
//const uint8_t OPC_EOR = 2;
//const uint8_t OPC_EON = 2;
//const uint8_t OPC_ORR = 1;
//const uint8_t OPC_ORN = 1;
//const uint8_t OPC_SUB = 2;
//const uint8_t OPC_SUBS = 3;
//const uint8_t OPC_MOVN = 0;
//const uint8_t OPC_MOVZ = 2;
//const uint8_t OPC_MOVK = 3;
//const uint8_t DPI_WIDE_MOVE_OPI = 5;
//const uint8_t DPI_ARITH_OPI = 2;
//const uint8_t DPI_IMM_26_28 = 4;
//const uint8_t DPI_REG_ARITH_LOGIC_25_28 = 5;
//const uint16_t DPI_MUL_21_30 = 216;
//
//
//
//uint32_t parse_two_operands_with_des(char * opcode, char **arguments) {
//    int8_t opc;
//    int8_t negate = 0; // holds the value for N
//    bool logicOp = false;
//    uint32_t result = 0;
//    if (strcmp(opcode,"add") == 0){
//        opc = OPC_ADD;
//    }else if (strcmp(opcode,"adds") == 0){
//        opc = OPC_ADDS;
//    }else if (strcmp(opcode,"sub") == 0){
//        opc = OPC_SUB;
//    }else if (strcmp(opcode,"subs") == 0){
//        opc = OPC_SUBS;
//    }else if (strcmp(opcode,"and") == 0){
//        opc = OPC_AND;
//        logicOp = true;
//    }else if (strcmp(opcode,"ands") == 0){
//        opc = OPC_ANDS;
//        logicOp = true;
//    }else if (strcmp(opcode,"bic") == 0){
//        opc = OPC_BIC;
//        logicOp = true;
//        negate = 1;
//    }else if (strcmp(opcode,"bics") == 0){
//        opc = OPC_BICS;
//        logicOp = true;
//        negate = 1;
//    }else if (strcmp(opcode,"eor") == 0){
//        opc = OPC_EOR;
//        logicOp = true;
//    }else if (strcmp(opcode,"eon") == 0){
//        opc = OPC_EON;
//        logicOp = true;
//        negate = 1;
//    }else if (strcmp(opcode,"orr") == 0){
//        opc = OPC_ORR;
//        logicOp = true;
//    }else if (strcmp(opcode,"orn") == 0){
//        opc = OPC_ORN;
//        logicOp = true;
//        negate = 1;
//    }else{
//        fprintf(stderr,"failed to parse opcode for\n '%s'\nas 2 operand with des\n",opcode);
//        exit(EXIT_FAILURE);
//    }
//    if (arguments[0][0] == 'x'){
//        appendBits(&result,1,1);
//    }
//    appendBits(&result,opc,2);
//    if (is_literal(arguments[2])) {
//        appendBits(&result,DPI_IMM_26_28,3);
//        appendBits(&result,DPI_ARITH_OPI,3);
//        appendBits(&result, parse_imm12(arguments[2],arguments[3]),13);
//        appendBits(&result, parse_register(arguments[1]),REGISTER_ADR_SIZE);
//        appendBits(&result, parse_register(arguments[0]),REGISTER_ADR_SIZE);
//    } else {
//        appendBits(&result,DPI_REG_ARITH_LOGIC_25_28,4);
//        if (logicOp) {
//            appendBits(&result,0,1);
//        } else {
//            appendBits(&result,1,1);
//        }
//        uint8_t *parsedShift = parse_shift(arguments[3]);
//        appendBits(&result,parsedShift[0],2);
//        appendBits(&result,negate,1);
//        appendBits(&result, parse_register(arguments[2]),REGISTER_ADR_SIZE);
//        appendBits(&result,parsedShift[1],6);
//        appendBits(&result, parse_register(arguments[1]),REGISTER_ADR_SIZE);
//        appendBits(&result, parse_register(arguments[0]),REGISTER_ADR_SIZE);
//    }
//    return result;
//}
//
//uint32_t parse_multiply(char * opcode, char **arguments){
//    uint32_t result = 0;
//    if (arguments[0][0] == 'x'){
//        appendBits(&result,1,1);
//    }
//    appendBits(&result,DPI_MUL_21_30,10);
//    appendBits(&result, parse_register(arguments[2]),REGISTER_ADR_SIZE);
//    if (strcmp(opcode,"madd") == 0){
//        appendBits(&result,0,1);
//    }else if (strcmp(opcode,"msub") == 0){
//        appendBits(&result,1,1);
//    }else{
//        fprintf(stderr,"failed to parse opcode for\n'%s'\nas multiply\n",opcode);
//        exit(EXIT_FAILURE);
//    }
//    appendBits(&result, parse_register(arguments[3]),REGISTER_ADR_SIZE);
//    appendBits(&result, parse_register(arguments[1]),REGISTER_ADR_SIZE);
//    appendBits(&result, parse_register(arguments[0]),REGISTER_ADR_SIZE);
//    return result;
//}
//
//uint32_t parse_wide_move(char * opcode, char **arguments){
//    uint32_t result = 0;
//    if (arguments[0][0] == 'x'){
//        appendBits(&result,1,1);
//    }
//    if (strcmp(opcode,"movn") == 0){
//        appendBits(&result,OPC_MOVN,2);
//    }else if (strcmp(opcode,"movz") == 0){
//        appendBits(&result,OPC_MOVZ,2);
//    }else if (strcmp(opcode,"movk") == 0){
//        appendBits(&result,OPC_MOVK,2);
//    }else{
//        fprintf(stderr,"failed to parse opcode for\n'%s'\nas wide move\n",opcode);
//        exit(EXIT_FAILURE);
//    }
//    appendBits(&result,DPI_IMM_26_28,3);
//    appendBits(&result,DPI_WIDE_MOVE_OPI,3);
//    appendBits(&result, parse_imm16(arguments[1],arguments[2]),18);
//    appendBits(&result, parse_register(arguments[0]),REGISTER_ADR_SIZE);
//    return result;
//}
//
