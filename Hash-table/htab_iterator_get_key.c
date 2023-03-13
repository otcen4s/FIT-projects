/*
 * =====================================================================================
 *
 *       Filename:  htab_iterator_get_key.c
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
#include "private_htab_s.h"

const char * htab_iterator_get_key(htab_iterator_t it){
    bool valid = htab_iterator_valid(it);
    if (valid && it.ptr->key != NULL){
        return it.ptr->key;
    }
    return it.ptr->key;
}