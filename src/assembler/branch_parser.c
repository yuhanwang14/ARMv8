#include "branch_parser.h"

uint32_t parse_uncond(char *opcode, char **arguments, int32_t currentLoc){
    uint32_t result = 5; // pos 26-31
    appendBits(&result, parse_imm_general(arguments[0]),26); // simm 26, pos 0 - 25
    return result;
}

uint32_t parse_with_reg(char *opcode, char **arguments, int32_t currentLoc){
    uint32_t result = 3508160; // pos 10 - 31
    appendBits(&result, parse_register(arguments[0]), REGISTER_ADR_SIZE); // xn, pos 5 - 9
    appendBits(&result, 0, 5); // pos 0-4
    return result;
}

uint32_t parse_cond(char *opcode, char **arguments, int32_t currentLoc){
    uint32_t result = 84; //pos 24-31
    appendBits(&result, parse_imm_general(arguments[0]), 19); // simm19, pos 5 - 23
    appendBits(&result, 0, 1); // pos 4
    if (strcmp(opcode, "beq") == 0){ // cond, pos 0 - 4
        appendBits(&result, 0, 4); 
    }else if (strcmp(opcode,"bne") == 0){
        appendBits(&result, 1, 4);
    }else if (strcmp(opcode,"bge") == 0){
        appendBits(&result, 10, 4);
    }else if (strcmp(opcode,"blt") == 0){
        appendBits(&result, 11, 4);
    }else if (strcmp(opcode,"bgt") == 0){
        appendBits(&result, 12, 4);
    }else if (strcmp(opcode,"ble") == 0){
        appendBits(&result, 13, 4);
    }else if (strcmp(opcode,"bal") == 0){
        appendBits(&result, 14, 4);
    }else{
        fprintf(stderr, "failed to parse the condition of '%s'\n", opcode);
        exit(EXIT_FAILURE);
    }
    return result;

}



uint32_t branch_parser(char *opcode, char **arguments, int32_t currentLoc){
    if (*opcode != 'b'){
        fprintf(stderr, "failed to parse '%s' as branching instruction\n",opcode);
            exit(EXIT_FAILURE);
    }
    switch (strlen(opcode)){
        case 1:
            return parse_uncond(opcode, arguments, currentLoc);
        case 2:
            return parse_with_reg(opcode, arguments, currentLoc);
        case 3:
            return parse_cond(opcode, arguments, currentLoc);
        default:
            fprintf(stderr, "failed to parse '%s' as branching instruction\n",opcode);
            exit(EXIT_FAILURE);
    }
}
