/*
 * =====================================================================================
 *
 *       Filename:  htab_hash_function.c
 *
 *    Description:  
 *
 *        Version:  1.1
 *        Created:  24.03.2019
 *       Revision:  none
 *       Compiler:  gcc 7.3.0
 *
 *         Author:  MATEJ OTCENAS 
 *  
 * =====================================================================================
 */

#include "htab.h"
#include "stdint.h"

#ifdef HASHTEST
    unsigned long sdbm(unsigned char* str){
        unsigned long hash = 0;
        int c;

        while ((c = *str++))
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
    }
#else

unsigned int htab_hash_function(const char *str){
    uint32_t h=0;     // has to be 32-bit
        const unsigned char *p;
        for(p=(const unsigned char*)str; *p!='\0'; p++)
            h = 65599*h + *p;
        return h;
}
#endif

