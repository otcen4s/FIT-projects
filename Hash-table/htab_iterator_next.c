/*
 * =====================================================================================
 *
 *       Filename:  htab_iterator_next.c
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

htab_iterator_t htab_iterator_next(htab_iterator_t it){
    struct htab_item *nextItem = it.ptr;
    if(it.ptr->next != NULL){
        nextItem = it.ptr->next;
        it.ptr = nextItem;
        return it;
    }

    for (size_t i = it.idx + 1; i < htab_bucket_count(it.t); i++){
        it.ptr = it.t->array[i];
        nextItem = it.ptr;
        
        if(nextItem != NULL){
            it.idx = i;
            it.ptr = nextItem;
            return it;
        }
    }
    return htab_end(it.t);  
}