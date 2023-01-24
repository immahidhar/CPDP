# Name: Mahidhar Reddy Narala (MN22L)
# Class: COP 5570
# Professor: Zhenhai Duan
# Project: 1
# Description: This is the Project 1 makefile.

C = gcc
CFLAGS = -std=gnu99 -Wall -pedantic -g

TARGETS = clean mytree build
BINARIES = mytree.o

default: $(TARGETS)

mytree: mytree.c
	$(C) $(CFLAGS) -c mytree.c

build:
	$(C) $(CFLAGS) -o mytree.x $(BINARIES)

clean:
	-rm -f *.o *.x
