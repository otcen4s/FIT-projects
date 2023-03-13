CC=gcc
CFLAGS = -O2 -std=c11 -pedantic -Wall -Wextra -lm

all: primes primes-i steg-decode

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c

ppm.o: ppm.c ppm.h 
	$(CC) $(CFLAGS) -c ppm.c

eratosthenes.o: eratosthenes.c bit_array.h
	$(CC) $(CFLAGS) -c eratosthenes.c -lm

primes: primes.c error.o  eratosthenes.o bit_array.h 
	$(CC) $(CFLAGS) error.o eratosthenes.o primes.c -o primes -lm

primes-i: primes.c error.o eratosthenes.o bit_array.h
	$(CC) $(CFLAGS) -DUSE_INLINE error.o eratosthenes.o primes.c -o primes-i -lm

steg-decode: steg-decode.c eratosthenes.o ppm.o error.o bit_array.h
	$(CC) $(CFLAGS)  steg-decode.c eratosthenes.o error.o ppm.o -o steg-decode -lm

run: primes primes-i
	time ./primes
	time ./primes-i

clean:
	rm *.o 
clean-all:
	rm -f *.o primes primes-i steg-decode

zip: bit_array.h error.h error.c eratosthenes.h eratosthenes.c primes.c ppm.h ppm.c
	zip xotcen01.zip *.c *.h Makefile
