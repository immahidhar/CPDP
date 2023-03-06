C = gcc
CFLAGS = -std=gnu99 -Wall -pedantic -g
NAME = mytoolkit.x
OTHER = mypwd
TARGETS = clean processor executor cmd util main mypwd mytimeout build
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

mypwd: mypwd.c
	$(C) $(CFLAGS) mypwd.c -o mypwd

mytimeout: mytimeout.c
	$(C) $(CFLAGS) mytimeout.c -o mytimeout.x

build: $(BINARIES)
	$(C) $(CFLAGS) -o $(NAME) $(BINARIES)

clean:
	-rm -rf *.out *.o *.x *.dSYM $(NAME) $(OTHER)