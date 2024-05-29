#include <stdlib.h>
#include "parser.h"

int main(void) {
    printf("init\n");
    char code[24] = "str x1,[x0, #0x1]!";
    printf("%i\n",parse_instruction(code,0));
    return 0;
}
