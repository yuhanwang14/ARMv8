#include "parser.h"

int main(void) {
    printf("init\n");
    char code[24] = ".int 0x4141";
    printf("%i\n",parse_instruction(code,5));
    return 0;
}
