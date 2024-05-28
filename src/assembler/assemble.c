#include <stdlib.h>
#include "parser.h"

int main(void) {
    printf("init\n");
    char code[24] = "mul x3,x1,x2";
    printf("%i\n",parse_instruction(code,0));
    return 0;
}
