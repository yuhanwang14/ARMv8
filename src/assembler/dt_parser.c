#include "dt_parser.h"

const int8_t SDT_LITERAL_24_29 = 24;
const int8_s SDT_NOT_LITERAL_25_29 = 28;

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

uint32_t dt_parser(char *instruction, uint32_t currentLoc){
    char *opcode = strtok(instruction," ");
    char *rt = strtok(NULL,",");
    char *rest = strtok(NULL,"");
    if (*rest = '#') return parse_dt_literal(rt,rest,currentLoc);
    rest = rest + 1 // if we reach here, it implies the original str starts with '['
    bool postIndex = rest[3] == ']' // this is used to distingush Unsigned offset and post-index
    char *nextArg = strtok(rest,",] ");
    char *addressArg[3];
    int8_t numArg = 0;
    while (nextArg != NULL && numArg <3){
        addressArg[numArg] = nextArg;
        numArg ++;
        nextArg = strtok(NULL,",] ");
    }
    int32_t result = 1; //pos 31
     if (rt == 'x'){
        appendBits(&result,1,1); // sf, pos 30
    }
    appendBits(&result,SDT_NOT_LITERAL_25_29,5); // pos 25-29
    switch (numArg){
        case 1:
            appendBits(&result,parse_unsigned(addressArg),20);
        break;
        case 2:
            if (postIndex){
                appendBits(&result,parse_indexed_or_reg(addressArg),20);
            }else{
                appendBits(&result,parse_unsigned(addressArg),20);
            }
            break;
            case 3:
            appendBits(&result,parse_indexed_or_reg(addressArg),20)
    }
    appendBits(&result,parse_register(rt),REGISTER_ADR_SIZE);
    return result



    }
    return result;
}