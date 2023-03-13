/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: sslsniff.h
 ***/

/**
 * Header guard
 */
#ifndef SSL_SNIFF_H
#define SSL_SNIFF_H


/**
 * Include of libraries (C/C++)
 */

#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <ctime>
#include <getopt.h>
#include <netinet/tcp.h>   //Deklaracie tcp
#include <netinet/ip.h>    //Deklaracie ip
#include <netinet/ip6.h>   // Deklaracie ip6
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pcap.h>
#include <string>
#include <map>
#include <cstddef>

/**
 * Protocol header lengths
 */
#define ETHERNET_HEADER_LEN 14
#define IPV6_HEADER_LEN 40
#define IPV4_HEADER_LEN 20
#define TLS_HEADER_LEN 5

/**
 * Macros of hexadecimal numbers in decimal values
 */
#define HEXA_14 20
#define HEXA_15 21
#define HEXA_16 22
#define HEXA_17 23
#define HEXA_04 4
#define HEXA_03 3
#define HEXA_02 2
#define HEXA_01 1
#define HEXA_00 0

/**
 * Macros of success and failure
 */
#define SUCCESS 0
#define FAILURE 1

/**
 * Extern variables for usage among modules
 */
extern char global_filename [1024];
extern char global_first_src_IP[IPV6_HEADER_LEN];
extern char global_first_src_IP_tmp[IPV6_HEADER_LEN];
extern int global_size;
extern int global_syn_cnt;
extern u_int global_src_port;
extern u_int global_dst_port;
extern std::string global_interface;
extern std::string global_output;
extern std::string global_SNI;
extern std::string global_time_sec;
extern std::map <u_int, std::string> global_SNI_assigned;
extern std::map <u_int, std::string> global_arrival_time;
extern std::map <u_int, bool> global_tcp_syn;
extern std::map <u_int, bool> global_client_hello_port;
extern std::map <u_int, bool> global_server_hello_port;
extern std::map <u_int, bool> global_counter_set;
extern std::map <u_int, int> global_FIN;
extern std::map <u_int, int> global_packet_count;
extern std::map <u_int, int> global_bytes_count;
extern std::map <u_int, int> global_tcp_packet_count;
extern std::map <u_int, double> global_duration;
extern std::map <u_int, char[INET6_ADDRSTRLEN]> global_src_ip;


/**
 * Function declarations
 */
pcap_t * set_interface(const char* device, bool interface);
void print_help();
void parse_args(int argc, char** argv);
void lookup_all_interfaces(pcap_if_t * all_interfaces);
void capture_packet(pcap_t *handle, bool interface);
void Callback(u_char *user_data, const struct pcap_pkthdr* pkthdr, const u_char* packet);
void handle_SSL(const u_char* payload, size_t payload_len);
void determine_address(char *src_IP, char* dst_IP, int type, const struct ip *ipv4_header, const struct ip6_hdr *ipv6_header);
void handle_packet(const struct pcap_pkthdr *pkthdr, const u_char *packet);
void print_info(char* src_IP, char* dst_IP, u_int src_port, bool out);
void clear_buffers();
std::string print_time(struct timeval ts);
std::string get_SNI(const u_char *byte_ptr, size_t payload_len);
std::string parse_extensions(const u_char *byte_ptr, size_t payload_len);
std::string parse_server_name_extension(const u_char *byte_ptr, size_t payload_len);
bool determine_protocol(int type, const struct ip *ipv4_header, const struct ip6_hdr *ipv6_header, u_int* src_port,  const struct tcphdr *tcp_header, const u_char *packet, const struct pcap_pkthdr *pkthdr);
bool handle_TCP(u_int *src_port, const struct tcphdr *tcp_header, const u_char *packet, int len, const struct pcap_pkthdr *pkthdr);
const char* str_to_char(std::string str);
const u_char* shift_ptr(const u_char *byte_ptr, size_t len);
int hex_to_dec(char hexVal[]);
int determine_IP(const struct ether_header* ethernet_header);
int get_bytes_count(const u_char *byte_ptr, size_t payload_len);
size_t shift_hdr_len(const u_char *byte_ptr, size_t payload_len);

#endif //SSL_SNIFF_H