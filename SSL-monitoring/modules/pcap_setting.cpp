/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: pcap_setting.cpp
 ***/

#include "../headers/sslsniff.h"

/**
 * Function is setting the given device using pcap library.
 * Function includes the pcap filter set to 'tcp'.
 * Handling the offline or online connection -> e.g. file or interface.
 */
pcap_t* set_interface(const char* device, bool interface){                    
    pcap_t *handle;		            // error handler (check for NULL)
    char errbuf[PCAP_ERRBUF_SIZE];  // error buffer for stderr message

    struct bpf_program filter;
    char filter_exp[4] = "tcp";
    bpf_u_int32 ip = 0;

    if(interface){   
        // handling the interface given -> 'wlp5s0', 'enp3s0', ...
        handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);  // opens interface and determines the number of bytes for reading
        if (handle == NULL) {
            fprintf(stderr, "Couldn't open online device %s: %s\n", device, errbuf);
            exit(EXIT_FAILURE);
        }
    }

    else{
        // handling the file reading -> 'filename.pcapng' will be processed 
        handle = pcap_open_offline(device, errbuf);
        if (handle == NULL) {
            fprintf(stderr, "Couldn't open offline device %s: %s\n", device, errbuf);
            exit(EXIT_FAILURE);
        }
    }

    // compiling the given filter expression
    if (pcap_compile(handle, &filter, filter_exp, 0, ip) == -1) {
        printf("Bad filter - %s\n", pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    // setting the filter
    if (pcap_setfilter(handle, &filter) == -1) {
        printf("Error setting filter - %s\n", pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    return handle;
}

/**
 * Function captures each packet using either pcap_loop including callback function or while loop including pcap next packet function.
 */
void capture_packet(pcap_t *handle, bool interface) {
    if(interface) pcap_loop(handle, 0, Callback, NULL);
    
    else{
        struct pcap_pkthdr *pkthdr;
        const u_char *packet;

        while(pcap_next_ex(handle, &pkthdr, &packet) >= 0){
            // handling each packet
            handle_packet(pkthdr, packet);
        }
    }
    
    // closing the created pcap connection
    pcap_close(handle);
}


/**
 * Callback function is invoked whenever the new packet comes.
 */
void Callback(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet){
    // handling packet
    handle_packet(pkthdr, packet);
}
