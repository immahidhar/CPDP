CC = gcc
CFLAGS = -Wall -pthread
DEPS = 
OBJ_SERVER = server.o
OBJ_CLIENT = client.o
OBJ = $(OBJ_SERVER) $(OBJ_CLIENT)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server client

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(OBJ_CLIENT)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) server client