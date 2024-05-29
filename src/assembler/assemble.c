#include <stdlib.h>
#include "parser.h"

int main(void) {
    printf("init\n");
    char code[24] = "ldr x2,[x0]";
    printf("%i\n",parse_instruction(code,0));
    return 0;
}
