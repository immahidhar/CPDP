C = gcc
CFLAGS = -std=gnu99 -g

mytree: mytree.c
	$(C) $(CFLAGS) -c mytree.c
	$(C) $(CFLAGS) -o mytree.x mytree.o

mytime: mytime.c
	$(C) $(CFLAGS) -c mytime.c
	$(C) $(CFLAGS) -o mytime.x mytime.o

mymtimes: mymtimes.c
	$(C) $(CFLAGS) -c mymtimes.c
	$(C) $(CFLAGS) -o mymtimes.x mymtimes.o

clean:
	-rm -f *.o *.x