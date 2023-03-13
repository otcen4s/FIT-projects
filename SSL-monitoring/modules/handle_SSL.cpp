/***
 *** Author: Matej Otčenáš
 *** 28.10.2020
 *** Filename: handle_SSl.cpp
 ***/

#include "../headers/sslsniff.h"


/**
 * Function handles the SSL/TLS protocol.
 * The Client Hello and Server Hello are the important parts of protocol communication which have to be resolved.
 * Number of bytes are counted as well as the packet count.
 */
void handle_SSL(const u_char* payload, size_t payload_len){
    const u_char *byte_ptr = payload;
   
   // Handshake
    if(HEXA_16 == (int)*byte_ptr){
        if((size_t)*shift_ptr(byte_ptr, 1) == HEXA_03 && (size_t)*shift_ptr(byte_ptr, 2) <= HEXA_04){
                // it's Client Hello
                if((size_t)*shift_ptr(byte_ptr, 5) == HEXA_01){  
                    global_SNI = get_SNI(byte_ptr, payload_len);

                    strcpy(global_first_src_IP, global_first_src_IP_tmp);
                    
                    global_SNI_assigned[global_src_port] = global_SNI;
                    global_client_hello_port[global_src_port] = true;
                    global_bytes_count[global_src_port] = 0;
                }
                // it's Server Hello
                else if((size_t)*shift_ptr(byte_ptr, 5) == HEXA_02){  
                    global_server_hello_port[global_dst_port] = true;
                }
        }
    }
    
    // Finding the bytes count and packet count is incremented
    if(global_client_hello_port[global_src_port]) {
        global_packet_count[global_src_port] += 1;
        global_bytes_count[global_src_port] += payload_len - get_bytes_count(payload, payload_len);
        
    }
    else if(global_client_hello_port[global_dst_port]) {
             global_packet_count[global_dst_port] += 1;
             global_bytes_count[global_dst_port] += payload_len - get_bytes_count(payload, payload_len);
            
    }
}