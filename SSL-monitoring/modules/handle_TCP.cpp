/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: handle_TCP.cpp
 ***/

#include "../headers/sslsniff.h"

/**
 * TCP packet is handled properly.
 * Function finds the payload of the TCP if there is any.
 * Checks the TCP flags to determine the beginning and the end of the TCP communication on the concrete port.
 * It determines the port numbers
 */
bool handle_TCP(u_int *src_port, const struct tcphdr *tcp_header, const u_char *packet, int len, const struct pcap_pkthdr *pkthdr){
    // finding TCP header
    tcp_header = (struct tcphdr*)(packet + global_size);

    // port determination
    *src_port = ntohs(tcp_header->source);
    global_dst_port = ntohs(tcp_header->dest);
    global_src_port = *src_port;

    // finding the TCP payload and payload length
    const u_char* tcp_hdr = packet + global_size;
    int tcp_header_length =  (((*(tcp_hdr + 12)) & 0xF0) >> 4) * 4;
    global_size += tcp_header_length;
    const u_char* payload;
    payload = packet + global_size;
    size_t payload_len = len - global_size;

    // checking the flags -> flags are stored on 13th byte -> using mask we can get the concrete bit
    auto FIN = ((*(tcp_hdr + 13)) & 1);
    auto SYN = ((*(tcp_hdr + 13)) & 2);
    auto ACK = ((*(tcp_hdr + 13)) & 16);
    auto RST = ((*(tcp_hdr + 13)) & 4);

    // TCP synchronize -> signify starting the communication
    if(SYN == 2){
        // TCP server ACK and SYN was sent
        if((ACK != 16 && global_syn_cnt == 0) || (ACK == 16 && global_syn_cnt == 1)){
            global_syn_cnt++;
        }

        // First SYN
        if(global_syn_cnt == 1){
                // Timestamp and duration_sec are handled with the first SYN
                global_arrival_time[global_src_port] = print_time(pkthdr->ts);
                global_duration[global_src_port] = pkthdr->ts.tv_sec * 1000000 + pkthdr->ts.tv_usec;  // Epoch time
                global_tcp_syn[global_src_port] = true;  // First SYN
                global_tcp_packet_count[global_src_port] = 0;
        }
        // Second SYN -> reset SYN count
        else if (global_syn_cnt == 2) {
            global_syn_cnt = 0;
        }
    }



    // FIN flag was set
    if(FIN == 1 && global_client_hello_port[global_src_port]) {
        global_FIN[global_src_port] += 1;
        
        // Second FIN flag was set -> signify the end of the communication
        // IP compare is due to client repeated FIN message if the previous packet has lost
        if(global_FIN[global_src_port] == 2  && global_server_hello_port[global_src_port] && !strcmp(global_src_ip[global_src_port], global_first_src_IP)){
            global_tcp_syn[global_src_port] = false;
            global_packet_count[global_src_port]++;
            global_duration[global_src_port] = ((pkthdr->ts.tv_sec * 1000000 + pkthdr->ts.tv_usec) - global_duration[global_src_port]) / 1000000;
            
            // output will be printed to stdout
            return true;
        }
    }

    // FIN flag was set
    else if (FIN == 1 && global_client_hello_port[global_dst_port]) {
        global_FIN[global_dst_port] += 1;
        
        // Second FIN flag was set -> means the end of the communication
        // IP compare is due to client repeated FIN message if the previous packet has lost
        if(global_FIN[global_dst_port] == 2 && global_server_hello_port[global_dst_port] && !strcmp(global_src_ip[global_dst_port], global_first_src_IP)){
            global_tcp_syn[global_dst_port] = false;
            global_duration[global_dst_port] = ((pkthdr->ts.tv_sec * 1000000 + pkthdr->ts.tv_usec) - global_duration[global_dst_port]) / 1000000;
            global_packet_count[global_dst_port]++;   
            
            // output will be printed to stdout
            return true;
        }
    }

    // RST flag was set -> it is considered the same way as if two TCP FIN packets were received
    // which means the end of communication
    // In the TCP communication was probably something wrong but the program still assumes that the SSL monitoring is correct
    if (RST == 4 && global_client_hello_port[global_src_port] && global_server_hello_port[global_src_port]){   
        global_tcp_syn[global_src_port] = false;
        global_duration[global_src_port] = ((pkthdr->ts.tv_sec * 1000000 + pkthdr->ts.tv_usec) - global_duration[global_src_port]) / 1000000;
        global_packet_count[global_src_port]++;   
        
        // output will be printed to stdout
        return true;
    }
    else if (RST == 4 && global_client_hello_port[global_dst_port] && global_server_hello_port[global_dst_port]) {
        global_tcp_syn[global_dst_port] = false;
        global_duration[global_dst_port] = ((pkthdr->ts.tv_sec * 1000000 + pkthdr->ts.tv_usec) - global_duration[global_dst_port]) / 1000000;
        global_packet_count[global_dst_port]++;   
            
        // output will be printed to stdout
        return true;
    }

    // indicates that there is payload after TCP header => we want to check if it is SSL/TLS layer
    if(payload_len > 0 && payload_len > TLS_HEADER_LEN + 1){ // + 1 -> due to ethernet padding
        handle_SSL(payload, payload_len);
    }
    // no TCP payload
    else{
        if(global_tcp_syn[global_src_port]){
            global_packet_count[global_src_port]++;
        }
        else if (global_tcp_syn[global_dst_port]) {
            global_packet_count[global_dst_port]++;
        }
    }

    return false;
}
