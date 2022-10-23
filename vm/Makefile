C_SOURCES = $(wildcard src/*.c)
CC        = /usr/bin/clang
CFLAGS    = -I ./src/include -Wall -Wextra -lcsfml-graphics -lcsfml-window
DEBUGGER  = /usr/bin/gdb

debug:
	$(CC) $(CFLAGS) -g $(C_SOURCES) -o ../bin/celeste.out
	$(DEBUGGER) ../bin/celeste.out

all:
	$(CC) $(CFLAGS) $(C_SOURCES) -o ../bin/celeste.out