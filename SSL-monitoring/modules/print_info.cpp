/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: print_info.cpp
 ***/

#include "../headers/sslsniff.h"

/**
 * Function prints info about TLS/SSL connection to stdout.
 */
void print_info(char* src_IP, char* dst_IP, u_int src_port, bool out){
    if(out){
        // if the client_fin is true, the output will be printed from the client side view
        bool client_fin = false;
        // if the server_fin is true, the output will be printed from the server side view
        bool server_fin = false;

        for(auto& x : global_client_hello_port){
            if(x.first == src_port && x.second) {
                client_fin = true;
                break;
            }
            else if (x.first == global_dst_port && x.second) {
                server_fin = true;
                break;
            }
        }
    
        if(client_fin) {
            global_output += global_arrival_time[global_src_port];
            global_output += src_IP;
            global_output += ',';
            global_output += std::to_string(src_port) + ',';
            global_output += dst_IP;
            global_output += ',';
        }
        else if(server_fin) {
            global_output += global_arrival_time[global_dst_port];
            global_output += dst_IP;
            global_output += ',';
            global_output += std::to_string(global_dst_port) + ',';
            global_output += src_IP;
            global_output += ',';
        }
        
        for(auto& x : global_SNI_assigned){
            if(x.first == src_port) {
                global_output += x.second;
                break;
            }
            else if(x.first == global_dst_port) {
                global_output += x.second;
                break;
            }
        }
        
        global_output += ",";
        
        if(client_fin) {
            global_output += std::to_string(global_bytes_count[src_port]) + ',';
            global_output += std::to_string(global_packet_count[src_port]) + ',';
            global_output += std::to_string(global_duration[global_src_port]) + '\n';
        }
        else if(server_fin) {
            global_output += std::to_string(global_bytes_count[global_dst_port]) + ',';
            global_output += std::to_string(global_packet_count[global_dst_port]) + ',';
            global_output += std::to_string(global_duration[global_dst_port]) + '\n'; 
        }
    
        // setting the decimal point precission
        std::cout.precision(6);
        std::cout << std::fixed;
        std::cout<<global_output;
        global_output = "";

        // clear buffers after connection has terminated
       clear_buffers();
    }
 }