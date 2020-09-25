CC=g++
CFLAGS=-Wall -std=c++11 -g -o2 -pedantic -Wextra
RM=rm -f *.o

all: tdoasystem

tdoasystem: main.cpp tdoasystem.h
	$(CC) $(CFLAGS) main.cpp tdoasystem.h -o tdoasystem -lsimlib -lm

%.o : %.c %.h
		$(CC) $(CFLAGS) -c -g $<

run: tdoasystem
	./tdoasystem

clean:
	$(RM) tdoasystem
