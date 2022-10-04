C_SOURCES = $(wildcard src/*.c)
CC        = /usr/bin/clang
CFLAGS    = -I ./src/include -Wall -Wextra -g
DEBUGGER  = /usr/bin/gdb

debug:
	$(CC) $(CFLAGS) -g $(C_SOURCES) -o ../bin/assembler.out
	$(DEBUGGER) ../bin/assembler.out

all:
	$(CC) $(CFLAGS) $(C_SOURCES) -o ../bin/assembler.out