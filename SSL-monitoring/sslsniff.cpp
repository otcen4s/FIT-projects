/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: sslsniff.cpp
 ***/

#include "./headers/sslsniff.h"


/**
 * Definition of extern variables
 */
std::string global_interface = "";
std::string global_output = "";
std::string global_SNI = "";
std::string global_time_sec = "";
std::string SNI = "";  
int global_size = 0;
int global_syn_cnt = 0;
u_int global_src_port = -1;
u_int global_dst_port = -1;
char global_filename [1024] = "";
char global_first_src_IP[IPV6_HEADER_LEN];
char global_first_src_IP_tmp[IPV6_HEADER_LEN];
std::map <u_int, bool> global_client_hello_port;
std::map <u_int, std::string> global_SNI_assigned;
std::map <u_int, int> global_FIN;
std::map <u_int, bool> global_tcp_syn;
std::map <u_int, int> global_packet_count;
std::map <u_int, int> global_bytes_count;
std::map <u_int, double> global_duration;
std::map <u_int, int> global_tcp_packet_count;
std::map <u_int, bool> global_counter_set;
std::map <u_int, std::string> global_arrival_time;
std::map <u_int, bool> global_server_hello_port;
std::map <u_int, char[INET6_ADDRSTRLEN]> global_src_ip;


/****************** MAIN ********************/

/**
 * Main function starts the program.
 */
int main(int argc, char * argv []){
    // argument parsing
    parse_args(argc, argv);
   
    pcap_t *handle = NULL;

    // argument is "-i"
    if(strcmp(global_filename , "") == 0){
        const char* interface = str_to_char(global_interface);
        handle = set_interface(interface, true);
        capture_packet(handle, true);
    }
    // argument is "-r"
    else if (global_interface == ""){
        handle = set_interface(global_filename, false); 
        capture_packet(handle, false);
    }
    
    return SUCCESS;
}

/***************** END OF MAIN ***************/