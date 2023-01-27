# Name: Mahidhar Reddy Narala (MN22L)
# Class: COP 5570
# Professor: Zhenhai Duan
# Project: 1
# Description: This is the Project 1 makefile.

C = gcc
CFLAGS = -std=gnu99 -Wall -pedantic -g

TARGETS = clean mytree mytime build

default: $(TARGETS)

mytree: mytree.c
	$(C) $(CFLAGS) -c mytree.c

mytime: mytime.c
	$(C) $(CFLAGS) -c mytime.c

build:
	$(C) $(CFLAGS) -o mytree.x mytree.o
	$(C) $(CFLAGS) -o mytime.x mytime.o

clean:
	-rm -f *.o *.x
