CC=g++
CFLAGS=-Wall -std=c++11 -static-libstdc++ -pedantic -pthread -Wextra -g
RM=rm -f *.o

all: popser

popser: main.cpp main.hpp server.cpp server.hpp args.hpp args.cpp md5.cpp md5.hpp mail.cpp mail.hpp string.cpp string.hpp
	$(CC) $(CFLAGS) -g main.cpp main.hpp server.cpp server.hpp args.hpp args.cpp md5.cpp md5.hpp mail.cpp mail.hpp string.cpp string.hpp -o popser

%.o : %.c %.h
		$(CC) $(CFLAGS) -c $<

clean:
	$(RM) popser