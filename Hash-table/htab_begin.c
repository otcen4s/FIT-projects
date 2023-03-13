/*
 * =====================================================================================
 *
 *       Filename:  htab_begin.c
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

htab_iterator_t htab_begin(const htab_t * t){
    htab_iterator_t first;

    for (size_t i = 0; i < htab_bucket_count(t); i++){
        if (t->array[i] != NULL){
            first.t = t;
            first.idx = i;
            first.ptr = t->array[i];
            return first;   
        }
    }
    first.t = t;
    first.idx = 0;
    first.ptr = NULL;
    
    return first;
}