CC := g++
CPPFLAGS = -std=c++11 -Werror -Wall -g -lpcap

all: ipk-sniffer

ipk-sniffer: ipk-sniffer.cpp
	$(CC) $(CPPFLAGS) ipk-sniffer.cpp -o ipk-sniffer -lpcap

clean:
	rm -f ipk-sniffer *.o
	
tar: 
	tar -cvf xotcen01.tar ipk-sniffer.cpp ipk-sniffer.h manual.pdf README Makefile
