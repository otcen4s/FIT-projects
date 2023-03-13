/*
 * =====================================================================================
 *
 *       Filename:  htab.h
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

#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <string.h>     
#include <stdbool.h>    
#include <stdio.h>
#include <stdlib.h>

struct htab;    

typedef struct htab htab_t;     

struct htab_item;               

typedef struct htab_iterator {
    struct htab_item *ptr;      
    const htab_t *t;            
    int idx;                    
} htab_iterator_t;              

unsigned int htab_hash_function(const char *str);

//////////////////// ADDED HASH FUNCTION ////////////////////
unsigned long sdbm(unsigned char* str);
/////////////////////////////////////////////////////////////

htab_t *htab_init(size_t n);

htab_t *htab_move(size_t n, htab_t *from);

size_t htab_size(const htab_t * t);             

size_t htab_bucket_count(const htab_t * t);

htab_iterator_t htab_lookup_add(htab_t * t, const char *key);

htab_iterator_t htab_begin(const htab_t * t);

htab_iterator_t htab_end(const htab_t * t);

htab_iterator_t htab_iterator_next(htab_iterator_t it);

static inline bool htab_iterator_valid(htab_iterator_t it) { return it.ptr!=NULL; }

static inline bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2) {
  return it1.ptr==it2.ptr && it1.t == it2.t;
}

const char * htab_iterator_get_key(htab_iterator_t it);

int htab_iterator_get_value(htab_iterator_t it);

int htab_iterator_set_value(htab_iterator_t it, int val);

void htab_clear(htab_t * t);

void htab_free(htab_t * t);

#endif // __HTABLE_H__