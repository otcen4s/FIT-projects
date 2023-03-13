/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: tcp_ip_parser.cpp
 ***/

#include "../headers/sslsniff.h"


/**
 * Function determines the IP part of the OSI packet structure.
 * The goal is to find the IPv4 or IPv6.
 */
int determine_IP(const struct ether_header* ethernet_header){
    auto ether_type = ntohs(ethernet_header->ether_type);

    switch(ether_type){
        // IPv4
        case ETHERTYPE_IP:  
            return 4;
            break;

        // IPv6
        case ETHERTYPE_IPV6:  
            return 6;
            break;

        // We can ignore other types of protocol
	    default: 
            return -1;
    }

    return -1;
}

/**
 *  Function determines the source and destination IP address including IPv4 and IPv6.
 */
void determine_address(char *src_IP, char *dst_IP, int type, const struct ip *ipv4_header, const struct ip6_hdr *ipv6_header){
    if(type == 4){
        inet_ntop(AF_INET, &(ipv4_header->ip_src), src_IP, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ipv4_header->ip_dst), dst_IP, INET_ADDRSTRLEN);
    }
    else if(type == 6){
        inet_ntop(AF_INET6, &(ipv6_header->ip6_src), src_IP, INET6_ADDRSTRLEN);
		inet_ntop(AF_INET6, &(ipv6_header->ip6_dst), dst_IP, INET6_ADDRSTRLEN);
    }
    else return;
}

/**
 * Function determines the protocol type.
 * This function is looking for TCP protocol only.
 */
bool determine_protocol(int type, const struct ip *ipv4_header, const struct ip6_hdr *ipv6_header, u_int* src_port, const struct tcphdr *tcp_header, const u_char *packet, const struct pcap_pkthdr *pkthdr){
    // handling IPv4
    if(type == 4){
        switch(ipv4_header->ip_p){
            // TCP
            case IPPROTO_TCP:
                if(handle_TCP(src_port, tcp_header, packet, pkthdr->caplen, pkthdr)) return true;
                break;

            // We can ignore other types of protocol
            default:
                break;
        }

        return false;
    }
    // handling IPv6
    else if(type == 6){
        switch(ipv6_header->ip6_nxt){
            // TCP
            case IPPROTO_TCP:
                if(handle_TCP(src_port, tcp_header, packet, pkthdr->caplen, pkthdr)) return true;
                break;

            // We can ignore other types of protocol
            default:
                break;
        }
        return false;
    }
    else return false;
}
