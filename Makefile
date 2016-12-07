CC = gcc
CFLAGS = -g -Wall

all: server client

server: netfileserver.c libnetfiles.h
	$(CC) $(CFLAGS) netfileserver.c -o server -pthread

client: libnetfiles.c client.c libnetfiles.h
	$(CC) $(CFLAGS) libnetfiles.c client.c -o client

clean:
	rm client server
