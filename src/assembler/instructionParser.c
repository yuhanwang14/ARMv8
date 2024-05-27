#include "instructionParser.h"

int instructionParser(char *instruction, int sizeString) {

}

int twoOprParser(char *instruction, int sizeString) {
    char **buffer = NULL;
    char opcode[5];
    strcpy(opcode, strtok_r(instruction, " ", buffer));
    char *arguments[4];
    char opc[4];
    char negate = '0'; // holds the value for N
    bool logicOp = false;
    char result[33];
    for (int i = 0; i < 4; i++) {
        arguments[i] = strtok_r(NULL, " ", buffer);
    }
    switch (opcode[0]) {
        case 'a':
            if (opcode[1] == 'd') {
                // add or adds
                if (opcode[3] == 's') {
                    // adds
                    strcpy(opc, "01");
                } else {
                    // add
                    strcpy(opc, "00");
                }
            } else {
                // and or ands
                logicOp = true;
                if (opcode[3] == 's') {
                    // ands
                    strcpy(opc, "11");
                } else {
                    // and
                    strcpy(opc, "00");
                }
            }
            break;
        case 'b':
            // bic/bics
            logicOp = true;
            negate = '1';
            if (opcode[3] == 's') {
                //bics
                strcpy(opc, "11");
            } else {
                //bic
                strcpy(opc, "00");
            }
            break;
        case 'e':
            // eor/eon
            logicOp = true;
            strcpy(opc, "10");
            if (opcode[2] == 'n') {
                // eon
                negate = '1';
            }
            break;

        case 'o':
            // orr/orn
            logicOp = true;
            strcpy(opc, "01");
            if (opcode[2] == 'n')
                negate = '1'; // orn
            break;

        case 's':
            // sub/subs
            if (opcode[3] == 's') {
                // adds
                strcpy(opc, "11");
            } else {
                // add
                strcpy(opc, "10");
            }
            break;

        default:
            printf("could not parse %s\nerror occur at twoOprParser, opcode fails to parse", instruction);
            return -1;
    }
    result[0] = '1';
    strcpy(&result[1], opc);
    if (isImmediate(arguments[2])) {
        strcpy(&result[3], "100010"); //23
        strcpy(&result[9], immediate12Parser(arguments[2], arguments[3]));//10,next index 22
        strcpy(&result[22], fetchRegister(arguments[1]));
        strcpy(&result[27], fetchRegister(arguments[0]));
    } else {
        strcpy(&result[3], "0101");
        if (logicOp) {
            result[7] = '0';
            result[10] = negate;
        } else {
            result[7] = '1';
            result[10] = '0';
        }
        if (arguments[3] == NULL) {
            result[8] = '0';
            result[9] = '0';
        } else {
            switch (arguments[3][0]) {
                case 'l':
                    if (arguments[3][2] == 'l') {
                        // lsl
                        strcpy(&result[8], "00");
                    } else {
                        // lsr
                        strcpy(&result[8], "01");
                    }
                    break;
                case 'a':
                    strcpy(&result[8], "10"); // asr
                    break;
                case 'r':
                    strcpy(&result[8], "11"); // ror
                    break;
                default:
                    printf("could not parse %s\nerror occur at twoOprParser, shift fails to parse", instruction);
                    return -1;
            }
            result[10] = negate;
            strcpy(&result[11], fetchRegister(arguments[2]));
            if (arguments[3] == NULL){
                strcpy(&result[16],"000000");
            }else{
                char *temp = strtok(arguments[3]," ");
                temp = strtok(NULL," ");
                strcpy(&result[16], immediate6Parser(temp));
            }
            strcpy(&result[22], fetchRegister(arguments[1]));
            strcpy(&result[27], fetchRegister(arguments[0]));
        }
    }
    return flatten(result, 32);
}
