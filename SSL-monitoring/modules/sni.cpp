/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: sni.cpp
 ***/

#include "../headers/sslsniff.h"
#include <cstddef>

/**
 * Function parses the client hello TLS packet using pointer shift to obtain the server name from the TLS payload.
 * For more information follow this link: https://tls.ulfheim.net/
 */
std::string get_SNI(const u_char *byte_ptr, size_t payload_len){
    size_t pos = 38 + TLS_HEADER_LEN; // TLS header size + handshake header + client version + client random
    size_t len = 0;

    // SIGSEGV prevention
    if (pos + 1 > payload_len) return "";
    
    // Session ID 
    len = (size_t)*shift_ptr(byte_ptr, pos);
    pos += 1 + len;

    // SIGSEGV prevention
    if (pos + 2 > payload_len) return "";
    
    // Cipher Suites 
    len = ((size_t)*shift_ptr(byte_ptr, pos) << 8) + (size_t)*shift_ptr(byte_ptr, pos + 1);
    pos += 2 + len;

    // SIGSEGV prevention
    if (pos + 1 > payload_len) return "";
    
    // Compression Methods
    len = (size_t)*shift_ptr(byte_ptr, pos);
    pos += 1 + len;

    // SSLv3.0 does not contain the SNI -> no SNI resolved
    if (pos == payload_len && byte_ptr[1] == 3 &&  byte_ptr[2] == 0) {
        std::cerr<<("Received SSL 3.0 handshake without extensions");
        return "";
    }

    // SIGSEGV prevention
    if (pos + 2 > payload_len) return "";
    

    // Extensions
    len = ((size_t)*shift_ptr(byte_ptr, pos) << 8) + (size_t)*shift_ptr(byte_ptr, pos + 1);
    pos += 2;

    // SIGSEGV prevention
    if (pos + len > payload_len) return "";
    
    // parsing extensions
    return parse_extensions(byte_ptr + pos, len);
}

/**
 * Function parses a list of extensions in TLS payload.
 * Function obtains the extension length -> indicates the size of extension part. 
 */
std::string parse_extensions(const u_char *byte_ptr, size_t payload_len){
    size_t pos = 0;
    size_t len;

    while (pos + 4 <=  payload_len) {
        // Extension Length
        len = ((size_t)*shift_ptr(byte_ptr, pos + 2) << 8) + (size_t)*shift_ptr(byte_ptr, pos + 3);

        if ((size_t)*shift_ptr(byte_ptr, pos) == HEXA_00 && (size_t)*shift_ptr(byte_ptr, pos) == HEXA_00){
            // SIGSEGV prevention
            if (pos + 4 + len >  payload_len) return "";
            
            // server name parsing in extension part
            return parse_server_name_extension(byte_ptr + pos + 4, len);
        }
        pos += 4 + len;
    }

    return "";
}


/**
 * Function parses the server name from extension which contains the SNI -> server name e.g. "www.google.com", "www.vutbr.cz", ...
 */
std::string parse_server_name_extension(const u_char *byte_ptr, size_t payload_len){
    size_t pos = 2; 
    size_t len;
    std::string sni = "";

    // Extension - Server Name
    while(pos + 3 <  payload_len) {
        len = ((size_t)*shift_ptr(byte_ptr, pos + 1) << 8) + (size_t)*shift_ptr(byte_ptr, pos + 2);

        // SIGSEGV prevention
        if (pos + 3 + len >  payload_len) return "";
        
        // server name indicator has been found
        if((size_t)*shift_ptr(byte_ptr, pos) == HEXA_00){
            for(size_t i = 0; i < len; i++) {
                sni += (size_t)*shift_ptr(byte_ptr, pos + 3 + i);
            }
            sni[sni.length()] = '\0';

            return sni;
        }

        else std::cerr<< "Unknown server name extension name type\n";
    
        pos += 3 + len;
    }

    return "";
}