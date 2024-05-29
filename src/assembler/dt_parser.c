#include "dt_parser.h"

const int8_t SDT_LITERAL_24_29 = 24;
const int8_t SDT_NOT_LITERAL_25_29 = 28;

int32_t parse_simm19(char *absoluteAddress, uint32_t currentLoc){
    int32_t offset = atoi(absoluteAddress + 1) - currentLoc;
    if (offset > 0) return offset;
    return offset + (1 << 19);
}

uint32_t parse_dt_literal(char *rt, char *immediate, uint32_t currentLoc){
    uint32_t result = 0; // pos 31
    if (rt[0] == 'x'){
        appendBits(&result,1,1); // sf, pos 30
    }
    appendBits(&result,SDT_LITERAL_24_29,6); // pos 24-29
    appendBits(&result,parse_simm19(immediate,currentLoc),19); // simm 19, pos 5-23
    appendBits(&result,parse_register(rt),REGISTER_ADR_SIZE); // rt, pos 0-4
    return result;
}

uint32_t parse_indexed_or_reg(char *opcode, char **addressArg, int8_t numArg){
    uint32_t result = 0; // pos 23, 24 are both 0
    if (strcmp(opcode, "str") == 0){
        appendBits(&result, 0, 1); // 'L'bit, pos 22
    }else if (strcmp(opcode, "ldr") == 0){
        appendBits(&result, 1, 1);
    }else{
        fprintf(stderr, "failed to parse '%s' as a data transfer", opcode);
        exit(EXIT_FAILURE);
    }
    if (is_literal(addressArg[1])){
        // pre/post indexed
        appendBits(&result, 0, 1); // pos 21
        appendBits(&result, parse_imm12(addressArg[1],NULL), 9); // simm 9, pos 12-20
        if (numArg == 3){
            // pre-indexed
            appendBits(&result, 1 ,1);// 'I', pos 11
        }else{
            //post-indexed
            appendBits(&result, 0, 1);
        }
        appendBits(&result,1,1); // pos 10
    }else{
        appendBits(&result,1,1); // pos 21
        appendBits(&result, parse_register(addressArg[1]), REGISTER_ADR_SIZE); //xm, pos 16-20
        appendBits(&result,26,6); // pos 10-15
    }
    return result;
}

uint32_t parse_unsigned(char*opcode, char**addressArg,int8_t numArg){
    uint32_t result = 2; // the highest digit is '1' since it is an unsigned offset
    if (strcmp(opcode, "str") == 0){
        appendBits(&result, 0, 1); // 'L'bit, pos 22
    }else if (strcmp(opcode, "ldr") == 0){
        appendBits(&result, 1, 1);
    }else{
        fprintf(stderr, "failed to parse '%s' as a data transfer", opcode);
        exit(EXIT_FAILURE);
    }
    if (numArg == 1){
        // no immediate supplied
        appendBits(&result, 0, 12); // offset, pos 10-21
    }else{
        appendBits(&result, parse_imm12(addressArg[1],NULL),12);
    }
    appendBits(&result, parse_register(addressArg[0]), REGISTER_ADR_SIZE); // xn, pos 5-9
    return result;
}

uint32_t dt_parser(char *instruction, uint32_t currentLoc){
    char *opcode = strtok(instruction," ");
    char *rt = strtok(NULL,",");
    char *rest = strtok(NULL,"");
    if (*rest == '#') return parse_dt_literal(rt,rest,currentLoc);
    rest = rest + 1; // if we reach here, it implies the original str starts with '['
    bool postIndex = rest[3] == ']'; // this is used to distingush Unsigned offset and post-index
    char *nextArg = strtok(rest,",] ");
    char *addressArg[3];
    int8_t numArg = 0;
    while (nextArg != NULL && numArg <3){
        addressArg[numArg] = nextArg;
        numArg ++;
        nextArg = strtok(NULL,",] ");
    }
    uint32_t result = 1; //pos 31
     if (*rt == 'x'){
        appendBits(&result,1,1); // sf, pos 30
    }
    appendBits(&result,SDT_NOT_LITERAL_25_29,5); // pos 25-29
    switch (numArg){
        case 1:
        // Unsigned offset without a immediate offset
            appendBits(&result,parse_unsigned(opcode, addressArg, numArg),20);
        break;
        case 2:
            if (postIndex){
                // post indexed address
                appendBits(&result,parse_indexed_or_reg(opcode, addressArg,numArg),20);
            }else{
                // Unsigned offset with a immediate offset
                appendBits(&result,parse_unsigned(opcode, addressArg, numArg),20);
            }
            break;
            case 3:
            // pre-indexed address, the last argument would be '!'
            appendBits(&result,parse_indexed_or_reg(opcode, addressArg, numArg),20);
    }
    appendBits(&result,parse_register(rt),REGISTER_ADR_SIZE);
    return result;
    }
