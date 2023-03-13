/*
 * =====================================================================================
 *
 *       Filename:  htab_move.c
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

htab_t *htab_move(size_t n, htab_t *from){
    htab_t *t = htab_init(n);
    if(t == NULL) return NULL;

    int data;

    htab_iterator_t begin = htab_begin(from);
    htab_iterator_t end = htab_end(from);

    htab_iterator_t it;

    while(!(htab_iterator_equal(begin, end))){
        it = htab_lookup_add(t, htab_iterator_get_key(begin));
        if(it.ptr == NULL) return NULL;

        data = htab_iterator_get_value(begin);
        htab_iterator_set_value(it, data);

        begin = htab_iterator_next(begin);
    }

    htab_clear(from);

    return t;
}