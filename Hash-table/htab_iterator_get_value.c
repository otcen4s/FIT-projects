/*
 * =====================================================================================
 *
 *       Filename:  htab_iterator_get_value.c
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

int htab_iterator_get_value(htab_iterator_t it){
    bool valid = htab_iterator_valid(it);
    if(valid){
        return it.ptr->data;
    }
    return -1;
}