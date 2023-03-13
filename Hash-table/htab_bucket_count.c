/*
 * =====================================================================================
 *
 *       Filename:  htab_bucket_count.c
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

size_t htab_bucket_count(const htab_t * t){
    return t->arr_size;
}