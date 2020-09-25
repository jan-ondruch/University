CC = g++
CFLAGS = -Wall -Wextra -pedantic

all: server client

server: server.cpp
	$(CC) $(CFLAGS) -o $@ server.cpp

client: client.cpp
	$(CC) $(CFLAGS) -o $@ client.cpp

clean:
	rm -f *.o *.out client server