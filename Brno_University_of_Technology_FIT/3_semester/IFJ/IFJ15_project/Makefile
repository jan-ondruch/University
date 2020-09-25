CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
BIN=ifj
RM=rm -f


all: paska.o scaner.o garbage.o parser.o main.o expr.o interpret.o ial.o str.o
	$(CC) $(CFLAGS) -o $(BIN) paska.o scaner.o garbage.o parser.o main.o expr.o interpret.o ial.o str.o
	
clean:
	$(RM) *.o $(BIN)
