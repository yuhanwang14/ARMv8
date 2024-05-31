#include "parser.h"
#include <stdio.h>

int main(void) {
    printf("init\n");
    char code[] = "add x0 x0 x0";
    printf("%i\n",parse_instruction(code,5));
    return 0;
}
