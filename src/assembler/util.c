#include "util.h"

uint8_t *parse_shift(char *shiftArg){
    uint8_t *result = malloc(2 * sizeof(uint8_t));
    result[0] = 0;
    result[1] = 0;
    if (shiftArg == NULL) return result;
    char *command = strtok(shiftArg," ");
    char *value = strtok(NULL," ");
    if (strcmp(command,"lsl") == 0){
    }else if (strcmp(command,"lsr") == 0){
        result[0] = 1;
    }else if (strcmp(command,"asr") == 0){
        result[0] = 2;
    }else if (strcmp(command,"ror") == 0){
        result[0] = 3;
    }else{
        fprintf(stderr,"failed to parse shift for '%s'\n",shiftArg);
    }
    result[1] = parse_imm6(value);
    return result;
}

uint8_t parse_register(char *registerName){
    if (registerName[0] != 'w'&& registerName[0] != 'x'){
        fprintf(stderr,"failed to parse register name '%s'\n",registerName);
        exit(EXIT_FAILURE);
    }
    if (strcmp(registerName+1,"zr") == 0) return 31;
    return atoi(registerName + 1);
}

bool is_literal(char *target){
    return (*target == '#');
}

uint16_t parse_imm12(char *literal, char *shift){
    if (shift == NULL){
        return atoi((literal+1));
    }else if (strcmp(shift,"lsl #12") == 0){
        return (atoi((literal+1)) + (1<<12));
    }else{
        fprintf(stderr,"failed to parse a shift for imm12: '%s'\n",shift);
        exit(EXIT_FAILURE);
    }
}

uint8_t parse_imm6(char *literal){
    return atoi(literal+1);
}

void appendBits(uint32_t *target,uint32_t appended,uint32_t length){
    *target = ((*target) << length) + appended;
    return;
}
