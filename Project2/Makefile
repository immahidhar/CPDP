C = gcc
CFLAGS = -std=gnu99 -Wall -pedantic -g
NAME = mytoolkit
TARGETS = clean util main build
BINARIES = util.o main.o

default: $(TARGETS)

main: main.c
	$(C) $(CFLAGS) -c main.c

util: util.c
	$(C) $(CFLAGS) -c util.c

build:
	$(C) $(CFLAGS) -o $(NAME) $(BINARIES)

clean:
	-rm -f *.out *.o *.x $(NAME)