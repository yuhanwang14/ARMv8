#include "dt_parser.h"

const int8_t SDT_LITERAL_24_29 = 24;
const int8_t SDT_NOT_LITERAL_25_29 = 28;

int32_t parse_simm19(char *absoluteAddress, uint32_t currentLoc){
    int32_t offset = strtol(absoluteAddress + 1, NULL, 0) - currentLoc;
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
        fprintf(stderr, "failed to parse opcode '%s' as a data transfer", opcode);
        exit(EXIT_FAILURE);
    }
    if (is_literal(addressArg[1])){
        // pre/post indexed
        printf("dt_instruction parsed as indexed\n");
        appendBits(&result, 0, 1); // pos 21
        appendBits(&result, parse_imm_general(addressArg[1]), 9); // simm 9, pos 12-20
        if (numArg == 3){
            // pre-indexed
            appendBits(&result, 1 ,1);// 'I', pos 11
        }else{
            //post-indexed
            appendBits(&result, 0, 1);
        }
        appendBits(&result,1,1); // pos 10
    }else{
        printf("dt_instruction parsed as register\n");
        appendBits(&result,1,1); // pos 21
        appendBits(&result, parse_register(addressArg[1]), REGISTER_ADR_SIZE); //xm, pos 16-20
        appendBits(&result,26,6); // pos 10-15
    }
    appendBits(&result, parse_register(addressArg[0]), REGISTER_ADR_SIZE); // xn, pos 5-9
    printf("%i\n",result);
    return result;
}

uint32_t parse_unsigned(char*opcode, char**addressArg,int8_t numArg, bool is64){
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
        if (is64){
            appendBits(&result, parse_imm_general(addressArg[1])/8,12);
        }else{
            appendBits(&result, parse_imm_general(addressArg[1])/4,12);
        }
    }
    appendBits(&result, parse_register(addressArg[0]), REGISTER_ADR_SIZE); // xn, pos 5-9
    printf("%i\n",result);
    return result;
}

uint32_t parse_sdt(char *opcode, char *argument, uint32_t currentLoc){
    printf("dt_parser control pos 1\n");
    char *rt = strtok(argument," ,");
    char *rest = strtok(NULL,"");
    if (*rest == '#') return parse_dt_literal(rt,rest,currentLoc);
    while (*rest == ' ') rest ++; // removes all spaces at the front(if any)
    rest = rest + 1; // if we reach here, it implies the original str starts with '[', remove the bracket
    bool postIndex = rest[2] == ']'; // this is used to distingush Unsigned offset and post-index
    char *nextArg = strtok(rest,",] ");
    char *addressArg[3];
    int8_t numArg = 0;
    printf("dt_parser control pos 2\n");
    while (nextArg != NULL && numArg <3){
        addressArg[numArg] = nextArg;
        numArg ++;
        nextArg = strtok(NULL,",] ");
    }
    printf("dt_parser control pos 3\n");
    uint32_t result = 1; //pos 31
     if (*rt == 'x'){
        appendBits(&result,1,1); // sf, pos 30
    }
    appendBits(&result,SDT_NOT_LITERAL_25_29,5); // pos 25-29
    switch (numArg){
        case 1:
        // Unsigned offset without a immediate offset
        printf("dt_instruction parsed as unsigned offset\n");
            appendBits(&result,parse_unsigned(opcode, addressArg, numArg, *rt == 'x'),20); // pos 5-24
        break;
        case 2:
            if ((!postIndex) && is_literal(addressArg[1])){
                // Unsigned offset with a immediate offset
                printf("dt_instruction parsed as unsigned offset\n");
                appendBits(&result,parse_unsigned(opcode, addressArg, numArg, *rt == 'x'),20);
            }else{   
                printf("dt_instruction parsed as index or register\n");
                appendBits(&result,parse_indexed_or_reg(opcode, addressArg,numArg),20);
            }
            break;
            case 3:
            // pre-indexed address, the last argument would be '!'
            printf("dt_instruction parsed as index or register\n");
            appendBits(&result,parse_indexed_or_reg(opcode, addressArg, numArg),20);
    }
    appendBits(&result,parse_register(rt),REGISTER_ADR_SIZE);
    return result;
    }
