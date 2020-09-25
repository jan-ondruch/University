PRJ=webclient
#
PROGS=$(PRJ)
CC=g++
CFLAGS=-std=c++98 -pedantic -Wextra -Wall
#CFLAGS=-D_BSD_SOURCE -Wall -std=c99 -pedantic -g -lm -Wextra

all: $(PROGS)

$(PRJ): $(PRJ).cpp
	$(CC) -o $@ $(PRJ).cpp

clean:
	rm -f *.o *.out *.dump *.stackdump webclient.exe index.html testing_raw.html testing.html $(PROGS)