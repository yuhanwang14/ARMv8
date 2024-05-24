CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate

emulate: emulate.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

emulate.o: utils.o fetch.o decode.o

clean:
	$(RM) *.o emulate
	
