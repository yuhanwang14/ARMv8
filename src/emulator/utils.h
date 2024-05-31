#include "register.h"
#include <stdio.h>

// FILE *f = safe_open(path, "r");
//  - tries to open a file, will exit if it failed
FILE *safe_open(char *path, const char *mode);

// log_state(reg, f);
//  - log the state of the emulator according to spec p16-p17
void log_state(Register *reg, FILE *fd);

// turns a n bit integer to a proper 64 bit one
int64_t sign_extend(int64_t x, unsigned nbits);
