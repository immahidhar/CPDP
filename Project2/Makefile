C = gcc
CFLAGS = -std=gnu99 -Wall -pedantic -g
NAME = mytoolkit
TARGETS = clean processor executor cmd util main build
BINARIES = processor.o executor.o cmd.o util.o main.o

default: $(TARGETS)

main: main.c
	$(C) $(CFLAGS) -c main.c

processor: processor.c
	$(C) $(CFLAGS) -c processor.c

executor: executor.c
	$(C) $(CFLAGS) -c executor.c

util: util.c
	$(C) $(CFLAGS) -c util.c

cmd: cmd.c
	$(C) $(CFLAGS) -c cmd.c

build:
	$(C) $(CFLAGS) -o $(NAME) $(BINARIES)

clean:
	-rm -f *.out *.o *.x $(NAME)