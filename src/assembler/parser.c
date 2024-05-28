#include "parser.h"


int8_t resolveAlias(char **opcode, char **arguments,int8_t numArg) {
// this checks if any alias occur and change the opcode and arguments if so
char *defaultZero = malloc(4 * sizeof(char));
if (arguments[0][0] == 'x'){
   strcpy(defaultZero,"xzr");
   }else{
   strcpy(defaultZero,"wzr");
}
   if (strcmp(*opcode,"cmp") == 0){
       *opcode = "subs";
       insertChar(arguments,numArg,defaultZero,0);
       return numArg + 1;
   }else if (strcmp(*opcode,"cmn") == 0){
       *opcode = "adds";
       insertChar(arguments,numArg,defaultZero,0);
       return numArg + 1;
   }else if (strcmp(*opcode,"neg") == 0){
       *opcode = "sub";
       insertChar(arguments,numArg,defaultZero,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"negs") == 0){
       *opcode = "subs";
       insertChar(arguments,numArg,defaultZero,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"tst") == 0){
       *opcode = "ands";
       insertChar(arguments,numArg,defaultZero,0);
       return numArg + 1;
   }else if (strcmp(*opcode,"mvn") == 0){
       *opcode = "orn";
       insertChar(arguments,numArg,defaultZero,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"mov") == 0){
       *opcode = "orr";
       insertChar(arguments,numArg,defaultZero,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"mul") == 0){
       *opcode = "madd";
       insertChar(arguments,numArg,defaultZero,3);
       return numArg + 1;
   }else if (strcmp(*opcode,"mneg") == 0){
       *opcode = "mneg";
       insertChar(arguments,numArg,defaultZero,3);
       return numArg + 1;
   }
   return numArg;
}

uint32_t parse_instruction(char *instruction, uint32_t currentLoc) {
    printf("parse pos 1\n%s\n",instruction);
    char *opcode = strtok(instruction," ,");
    // if (strcmp(opcode,"ldr") == 0 || strcmp(opcode,"str") == 0) return dt_parser(instruction,currentLoc);
    char *nextArg = strtok(NULL," ,");
    char *arguments[5] = {NULL,NULL,NULL,NULL,NULL};
    int numArg = 0;
    while (nextArg != NULL && numArg < 5){
            printf("%s\n",nextArg);
        arguments[numArg] = nextArg;
        numArg++;
        nextArg = strtok(NULL," ,");
    }
    printf("init pos 2, arguments seprated successfully\n");
    numArg = resolveAlias(&opcode,arguments,numArg);
    printf("init pos 3, instruction treated as '%s %s, %s, %s,%s, %s'\n",opcode,arguments[0],arguments[1],arguments[2],arguments[3],arguments[4]);
    switch (numArg){
        case 5:
            return parse_two_operands_with_des(opcode,arguments);
        case 4:
        if(is_shift(arguments[2])){
                return parse_wide_move(opcode,arguments);
            }else{
                printf("parse mult control point -1 %s\n",arguments[3]);
                return parse_multiply(opcode,arguments);
            }
        case 3:
                return parse_two_operands_with_des(opcode,arguments);
        case 2:
                return parse_wide_move(opcode,arguments);
        default:
            fprintf(stderr,"instruction fails to parse\n'%s'\ndoes not have correct number of arguments",instruction);
            exit(EXIT_FAILURE);
    }
}
