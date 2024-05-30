#include "parser.h"
#include <stdint.h>


int8_t resolve_alias(char **opcode, char **arguments,int8_t numArg, char *buffer) {
// this checks if any alias occur and change the opcode and arguments if so
buffer = malloc(4 * sizeof(char));
if (arguments[0][0] == 'x'){
   strcpy(buffer,"xzr");
   }else{
   strcpy(buffer,"wzr");
}
   if (strcmp(*opcode,"cmp") == 0){
       *opcode = "subs";
       insertChar(arguments,numArg,buffer,0);
       return numArg + 1;
   }else if (strcmp(*opcode,"cmn") == 0){
       *opcode = "adds";
       insertChar(arguments,numArg,buffer,0);
       return numArg + 1;
   }else if (strcmp(*opcode,"neg") == 0){
       *opcode = "sub";
       insertChar(arguments,numArg,buffer,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"negs") == 0){
       *opcode = "subs";
       insertChar(arguments,numArg,buffer,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"tst") == 0){
       *opcode = "ands";
       insertChar(arguments,numArg,buffer,0);
       return numArg + 1;
   }else if (strcmp(*opcode,"mvn") == 0){
       *opcode = "orn";
       insertChar(arguments,numArg,buffer,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"mov") == 0){
       *opcode = "orr";
       insertChar(arguments,numArg,buffer,1);
       return numArg + 1;
   }else if (strcmp(*opcode,"mul") == 0){
       *opcode = "madd";
       insertChar(arguments,numArg,buffer,3);
       return numArg + 1;
   }else if (strcmp(*opcode,"mneg") == 0){
       *opcode = "mneg";
       insertChar(arguments,numArg,buffer,3);
       return numArg + 1;
   }
   return numArg;
}

uint32_t parse_instruction(char *instruction, uint32_t currentLoc) {
    printf("parse pos 1\n%s\n",instruction);
    char *opcode = strtok(instruction," ,");
    if (strcmp(opcode,"ldr") == 0 || strcmp(opcode,"str") == 0) return parse_sdt(opcode, strtok(NULL,""),currentLoc); //special handler for DTIs
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
    char *buffer;
    numArg = resolve_alias(&opcode,arguments,numArg, buffer);
    printf("init pos 3, instruction treated as '%s %s, %s, %s,%s, %s'\n",opcode,arguments[0],arguments[1],arguments[2],arguments[3],arguments[4]);
    uint32_t result;
    switch (numArg){
        case 5:
            result = parse_two_operands_with_des(opcode,arguments);
            break;
        case 4:
        if(is_shift(arguments[2])){
                result = parse_wide_move(opcode,arguments);
            }else{
                printf("parse mult control point -1 %s\n",arguments[3]);
                result = parse_multiply(opcode,arguments);
            }
            break;
        case 3:
                result = parse_two_operands_with_des(opcode,arguments);
                break;
        case 2:
                result = parse_wide_move(opcode,arguments);
                break;
        case 1:
            if (*opcode == '.'){
                 result = parse_dir(opcode, arguments);
            }else{
                result = branch_parser(opcode,arguments,currentLoc);
            }
            break;
        default:
            fprintf(stderr,"instruction fails to parse\n'%s'\ndoes not have correct number of arguments",instruction);
            exit(EXIT_FAILURE);
    }
    free(buffer);
    return result;
}

