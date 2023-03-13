CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -g
LDFLAGS=-lrt -pthread

all: proj2

proj2: proj2.c sync.h
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean: 
	rm -f proj2 proj2.out

zip:
	zip proj2.zip *.c *.h Makefile
