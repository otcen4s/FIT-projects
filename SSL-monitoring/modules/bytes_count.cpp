/***
 *** Author: Matej Otčenáš
 *** Date: 28.10.2020
 *** Filename: bytes_count.cpp
 ***/

#include "../headers/sslsniff.h"


/**
 * Function is looking for TLS header records by parsing payload of size payload length.
 * Each header represents 5 bytes that are substracted from payload length.
 * This approach gives us the exact number of bytes ommiting the headers.
 */
int get_bytes_count(const u_char *byte_ptr, size_t payload_len){
    const u_char *ptr = byte_ptr;
    size_t byte_count = 0;
    size_t shift = 0;
    size_t length = 0;
   

    while (byte_count++ < payload_len) {
        // Change Cipher Spec // Encrypt Alert // Handshake // Application Data
        if((size_t)*ptr == HEXA_14 || (size_t)*ptr == HEXA_15 || (size_t)*ptr == HEXA_16 || (size_t)*ptr == HEXA_17){ 
            if((size_t)*shift_ptr(ptr, 1) == HEXA_03 && (size_t)*shift_ptr(ptr, 2) <= HEXA_04){
                length += 5;
                
                shift = shift_hdr_len(ptr, payload_len);
                ptr = shift_ptr(ptr, shift + 4);
                byte_count += shift + 4;

                continue;
            }  
        }
        ptr++; 
    }
    return length;
}


/**
 * Function returns the number for pointer shift.
 * It extracts 2 bytes form TLS header record which represents the length of TLS payload.
 */
size_t shift_hdr_len(const u_char *byte_ptr, size_t payload_len){
    size_t shift = 0;
    size_t offset = 3;
    char buff[10];
    char tmp[10];

    sprintf(buff, "%02x", *shift_ptr(byte_ptr, offset));
    
    offset += 1;
    
    sprintf(tmp, "%02x", *shift_ptr(byte_ptr, offset));
    
    strcat(buff, tmp);
    

    shift = hex_to_dec(buff);

    offset += hex_to_dec(buff);
   
    if(offset > payload_len){
        shift = payload_len;
        return shift;
    }
    return shift;
}