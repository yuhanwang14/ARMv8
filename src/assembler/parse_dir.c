#include "parse_dir.h"

uint32_t parse_dir(char *opcode, char **arguments){
    if (strcmp(opcode, ".int")){
        return strtol(arguments[0], NULL, 0);
    }else{
        fprintf(stderr, "failed to parse '%s' as directive",opcode);
        exit(EXIT_FAILURE);
    }
}