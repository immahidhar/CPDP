C = gcc
CFLAGS = -std=gnu99 -Wall -pedantic -g
NAME = mytoolkit
TARGETS = clean cmd util main build
BINARIES = cmd.o util.o main.o

default: $(TARGETS)

main: main.c
	$(C) $(CFLAGS) -c main.c

util: util.c
	$(C) $(CFLAGS) -c util.c

cmd: cmd.c
	$(C) $(CFLAGS) -c cmd.c

build:
	$(C) $(CFLAGS) -o $(NAME) $(BINARIES)

clean:
	-rm -f *.out *.o *.x $(NAME)