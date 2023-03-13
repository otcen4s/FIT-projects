###
### Author: Matej Otčenáš
### Date: 28.10.2020
### Filename: Makefile
###

# quiet makefile using prefix '@'
CC = @g++

PATHM = modules/

PATHH = headers/

CPPFLAGS = -std=c++11 -pedantic -Wall -Wextra -g -O3 -Wno-unused-parameter -lpcap

MODULES = $(PATHM)sni.cpp $(PATHM)print_info.cpp $(PATHM)packet_parser.cpp $(PATHM)help_functions.cpp $(PATHM)handle_TCP.cpp $(PATHM)handle_SSL.cpp $(PATHM)bytes_count.cpp $(PATHM)pcap_setting.cpp $(PATHM)tcp_ip_parser.cpp

all: sslsniff

sslsniff: sslsniff.cpp $(MODULES)
	$(CC) $(CPPFLAGS) $^ -o $@ $(info    Compiling program...) -lpcap

clean:
	rm -f sslsniff *.o
	
tar: 
	tar -cvf xotcen01.tar sslsniff.cpp headers modules Makefile manual.pdf sslsniff.1