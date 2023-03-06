C = gcc
CFLAGS = -std=gnu99 -Wall -pedantic -g
OBJS = main.o

default: clean mytimeout main mytoolkit

mytimeout: mytimeout.c
	$(C) $(CFLAGS) mytimeout.c -o mytimeout.x

main: main.c
	$(C) $(CFLAGS) -c main.c

mytoolkit: $(OBJS)
	$(C) $(CFLAGS) $(OBJS) -o mytoolkit.x

clean:
	-rm -rf *.out *.o *.x *.dSYM