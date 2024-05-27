#include "parser.h"

int8_t resolveAlias(char **opcode, char **arguments,int8_t numArg) {
// this checks if any alias occur and change the opcode and arguments if so
    char defaultZero[4];
    if (arguments[0][0] == 'x'){
        strcpy(defaultZero,"xzr");
        }else{
        strcpy(defaultZero,"wzr");
    }
    if (strcmp(*opcode,"cmp")){
        *opcode = "subs";
        insertChar(arguments,numArg,defaultZero,0);
        return numArg + 1;
    }else if (strcmp(*opcode,"cmn")){
        *opcode = "adds";
        insertChar(arguments,numArg,defaultZero,0);
        return numArg + 1;
    }else if (strcmp(*opcode,"neg")){
        *opcode = "sub";
        insertChar(arguments,numArg,defaultZero,1);
        return numArg + 1;
    }else if (strcmp(*opcode,"negs")){
        *opcode = "subs";
        insertChar(arguments,numArg,defaultZero,1);
        return numArg + 1;
    }else if (strcmp(*opcode,"tst")){
        *opcode = "ands";
        insertChar(arguments,numArg,defaultZero,0);
        return numArg + 1;
    }else if (strcmp(*opcode,"mvn")){
        *opcode = "orn";
        insertChar(arguments,numArg,defaultZero,1);
        return numArg + 1;
    }else if (strcmp(*opcode,"mov")){
        *opcode = "orr";
        insertChar(arguments,numArg,defaultZero,1);
        return numArg + 1;
    }else if (strcmp(*opcode,"mul")){
        *opcode = "madd";
        insertChar(arguments,numArg,defaultZero,3);
        return numArg + 1;
    }else if (strcmp(*opcode,"mneg")){
        *opcode = "mneg";
        insertChar(arguments,numArg,defaultZero,3);
        return numArg + 1;
    }
    return numArg;
}

uint32_t parse_instruction(char *instruction) {
    char **buffer = NULL;
    char *opcode = strtok_r(instruction," ",buffer);
    char *nextArg = strtok_r(NULL,",",buffer);
    char *arguments[4];
    int numArg = 0;
    while (nextArg != NULL && numArg < 4){
        arguments[numArg] = nextArg;
        numArg += 1;
        nextArg = strtok_r(NULL,",",buffer);
    }
    numArg = resolveAlias(&opcode,arguments,numArg);
    switch (numArg){
        case 4:
            if (is_shift(arguments[3])){
                return parse_two_operands_with_des(opcode,arguments);
            }else{
                return parse_multiply(opcode,arguments);
            }
        case 3:
            if(is_shift(arguments[2])){
                return parse_wide_move(opcode,arguments);
            }else{
                return parse_two_operands_with_des(opcode,arguments);
            }
        default:
            fprintf(stderr,"instruction fails to parse\n'%s'\ndoes not have correct number of arguments",instruction);
            exit(EXIT_FAILURE);
    }
}
