/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: packet_parser
 ***/

#include "../headers/sslsniff.h"

/**
 * Function handles every packet from loopback.
 * Variables initialization and calling other functions is performed here.
 */
void handle_packet(const struct pcap_pkthdr *pkthdr, const u_char *packet){
    // structures initialization
    const struct ip *ipv4_header = NULL;                                               // ipv4 header
    const struct ip6_hdr *ipv6_header = NULL;                                          // ipv6 header
    const struct tcphdr *tcp_header = NULL;                                            // tcp header
    const struct ether_header* ethernet_header = (struct ether_header*)packet;         // ethernet header

    u_int src_port; // client port 
    bool out = false; // output is set to false
    
    global_SNI = "";
    global_src_port = -1;
    global_dst_port = -1;
    global_size = ETHERNET_HEADER_LEN; // ETHERNET header size -> 14 B

    // IP address buffers of size IPv6 (rather more than less size)
    char src_IP[INET6_ADDRSTRLEN];  
    char dst_IP[INET6_ADDRSTRLEN];   

    // Determining possible IPv4
    if(determine_IP(ethernet_header) == 4){
        ipv4_header = (struct ip*)(packet + global_size);

        determine_address(src_IP, dst_IP, 4, ipv4_header, ipv6_header);
        
         // store IP to temporary global
        strcpy(global_first_src_IP_tmp, src_IP);

        // IPv4 header size -> 20 B
        global_size += IPV4_HEADER_LEN;  
        
        out = determine_protocol(4, ipv4_header, ipv6_header, &src_port, tcp_header, packet, pkthdr);
    }
    // Determining possible IPv6
    else if(determine_IP(ethernet_header) == 6) {
        ipv6_header = (struct ip6_hdr*)(packet + global_size); 
        
        determine_address(src_IP, dst_IP, 6, ipv4_header, ipv6_header);

        // store IP to temporary global
        strcpy(global_first_src_IP_tmp, src_IP);
        
        // IPv6 header size -> 40 B
        global_size += IPV6_HEADER_LEN;   
        
        out = determine_protocol(6, ipv4_header, ipv6_header, &src_port, tcp_header, packet, pkthdr);    
    }

    if(global_client_hello_port[global_src_port]){
        strcpy(global_src_ip[global_src_port], src_IP);
    }

    // output
    print_info(src_IP, dst_IP, global_src_port, out);
}