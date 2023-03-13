/***
 *** Autor: Matej Otčenáš
 *** Datum: 15.4.2020
 *** Nazov suboru: ipk-sniffer.h
 ***/

#ifndef PROJECT2_PACKET_SNIFFER_H
#define PROJECT2_PACKET_SNIFFER_H

#include <iostream>
#include <string>
#include <getopt.h>
#include <string.h>

#include <pcap.h>
#include <pcap/pcap.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/udp.h>   //Deklaracie udp
#include <netinet/tcp.h>   //Deklaracie tcp
#include <netinet/ip.h>    //Deklaracie ip
#include <netinet/if_ether.h>


#define ETHERNET_HEADER_LEN 14
#define IPV6_HEADER_LEN 40
#define IPV4_HEADER_LEN 20
#define UDP_HEADER_LEN 8

class Sniffer {
public:
    Sniffer(){};
public:
    void print_help();
    void process_args(int argc, char** argv);
    void lookup_all_interfaces(pcap_if_t * all_interfaces);
    pcap_t * set_interface(char *device, bool filter_all);
    void capture_packet(pcap_t *handle);
    static void Callback(u_char *user_data, const struct pcap_pkthdr* pkthdr, const u_char* packet);
    static void print_tcp(const u_char* packet, int ethernet_header_length, int ip_header_length, const struct pcap_pkthdr* packet_header);
    static void print_udp(const u_char* packet, int ethernet_header_length, int ip_header_length, const struct pcap_pkthdr* packet_header);
    static std::string get_hostname(char ip[INET6_ADDRSTRLEN], sa_family_t family, u_int port);
    static void print_data(int length, const u_char* data);
};

#endif //PROJECT2_PACKET_SNIFFER_H