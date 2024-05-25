#include "register.h"
#include <stdio.h>

FILE *safe_open(char *path);

void log_state(Register *reg, FILE *fd);
