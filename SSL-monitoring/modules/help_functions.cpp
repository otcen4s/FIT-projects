/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: help_functions.cpp
 ***/

#include "../headers/sslsniff.h"

/**
 * Function converts std::string to char*
 */
const char* str_to_char(std::string str){
    const char *array = str.c_str();
    return array;
}

/**
 * Function handles the timeval structure to obtain the timestamp
 */
std::string print_time(struct timeval ts){
    struct tm time = *localtime(&ts.tv_sec);
    char buffer [80];
    char current_time[93] = "";
   
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time);
    sprintf(current_time, "%s.%06ld", buffer, ts.tv_usec);

    std::string s = current_time;
    s[s.length()] = '\0';
    s += ",";

    return s;
}

/**
 * Function prints help guide to stdout and terminates the program successfully
 */
void print_help() {
    std::cout <<
              "-i [interface]:      Interface for listening and catching packets.\n"
              "-r [filename]:       File specification in '.pcapng' format\n"
              "-h:                  Help guide for program usage.\n"
              "Usage:               For running this program use one of the given flags.\n"
              "                     Please use only one flag option at the time, otherwise an error will be raised.\n";
    exit(SUCCESS);
}


/**
 * Function parses the given arguments from stdin using getopt
 */
void parse_args(int argc, char **argv) {
    int opt = 0;

    while(true){
        opt = getopt(argc, argv, "::i:r:h"); 
        
        if(opt != -1) {
            switch(opt){
                case 'i':                    
                    if(optarg && optarg[0]) global_interface = optarg;
                    break;

                case 'r':                    
                    if(optarg && optarg[0]) strcpy(global_filename, optarg);
                    break;

                case 'h':
                    print_help();
                    break;

                case ':':
                    std::cerr << "option needs a value, see -h option" << std::endl;
                    exit(FAILURE);

                case '?':
                    std::cerr << "unknown option: " << optopt << std::endl;
                    exit(FAILURE);
                    break;
            }
        }
        else{
            if(global_interface != "" && strcmp(global_filename, "") != 0) exit(FAILURE);
            else if(global_interface != "" || strcmp(global_filename, "") != 0) return;

            std::cerr << "no flags given, see -h option." << std::endl;
            exit(FAILURE);
        }
   }
}

/**
 * Function converts hexadecimal number to decimal value
 */
int hex_to_dec(char hexVal[]) { 
    int len = strlen(hexVal); 
    int base = 1;   
    int dec_val = 0; 

    for (int i = 0; i < len; i++) {
        hexVal[i] = toupper(hexVal[i]);  
    } 
    
    for (int i = len - 1; i >= 0; i--) {    
        if (hexVal[i] >= '0' && hexVal[i] <= '9') { 
            dec_val += (hexVal[i] - 48) * base; 
            base = base * 16; 
        } 
  
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') { 
            dec_val += (hexVal[i] - 55) * base; 
            base = base * 16; 
        } 
    } 
      
    return dec_val; 
} 

/**
 * Function shifts pointer by given length
 */
const u_char* shift_ptr(const u_char *byte_ptr, size_t len){
    const u_char *tmp = byte_ptr;

    for (size_t i = 0; i < len; i++) {
        tmp++;
    }

    return tmp;
}

/**
 * Function clears all buffers of the concrete TLS connection
 */
void clear_buffers(){
     for(auto& x : global_FIN){
        if(x.first == global_src_port  && x.second == 2){
            global_client_hello_port.erase(global_src_port);
            global_server_hello_port.erase(global_src_port);
            global_SNI_assigned.erase(global_src_port);
            global_bytes_count.erase(global_src_port);
            global_FIN.erase(global_src_port);
            global_packet_count.erase(global_src_port);
            global_bytes_count.erase(global_src_port);
            global_duration.erase(global_src_port);
            break;
        }
        else if (x.first == global_dst_port && x.second == 2) {
            global_client_hello_port.erase(global_dst_port);
            global_server_hello_port.erase(global_dst_port);
            global_SNI_assigned.erase(global_dst_port);
            global_bytes_count.erase(global_dst_port);
            global_FIN.erase(global_dst_port);
            global_packet_count.erase(global_dst_port);
            global_bytes_count.erase(global_dst_port);
            global_duration.erase(global_dst_port);
            break;
        }
    }
}