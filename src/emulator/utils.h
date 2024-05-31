#include "register.h"
#include <stdio.h>

// tries to open a file, will exit if it failed
FILE *safe_open(char *path, const char *mode);

// log the state of the emulator according to spec p16-p17
void log_state(Register *reg, FILE *fd);
