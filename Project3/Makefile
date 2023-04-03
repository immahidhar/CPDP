CC = g++
CFLAGS = -Wall -pedantic -g
LIBS = -pthread\

default: clean common.o server.o client.o server.x client.x

common.o: common.cpp common.h 
	  $(CC) $(CFLAGS) -c common.cpp

server.o: server.cpp common.h server.h
	  $(CC) $(CFLAGS) -c server.cpp 

client.o: client.cpp common.h client.h
	  $(CC) $(CFLAGS) -c client.cpp 

server.x:  server.o common.o 
	  $(CC) $(CFLAGS) -o server.x server.o common.o ${LIBS}

client.x:  client.o common.o
	  $(CC) $(CFLAGS) -o client.x client.o common.o ${LIBS}

clean:
	-rm -rf *.out *.o *.x