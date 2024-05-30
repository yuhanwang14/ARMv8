#include <stdlib.h>
#include "parser.h"

int main(void) {
    printf("init\n");
    char code[24] = "br x28";
    printf("%i\n",parse_instruction(code,5));
    return 0;
}
