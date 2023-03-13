/*
 * =====================================================================================
 *
 *       Filename:  htab_lookup_add.c
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

   
htab_iterator_t htab_lookup_add(htab_t * t, const char *key){
    #ifdef HASHTEST
        unsigned int index = (sdbm(key) % htab_bucket_count(t));
    #else
        unsigned int index = (htab_hash_function(key) % htab_bucket_count(t)); // index for key 
    #endif

    struct htab_item *newItem = t->array[index];
    htab_iterator_t it;

    if(newItem == NULL){ // adding item 
        newItem = malloc(sizeof(struct htab_item));
        newItem->key = malloc(sizeof(char) * (strlen(key)+1));
        if(newItem == NULL || newItem->key == NULL){
            fprintf(stderr,"%s", "Could not allocate memory\n");
            it.t = t;
            it.ptr = NULL;
            it.idx = index; // or -1
            return it;
        }
        // created a new node (first node)
        newItem->next = NULL;
        memcpy(newItem->key, key, strlen(key)+1);
        newItem->data = 0;
        
        t->size++;
        t->array[index] = newItem;

        it.t = t;
        it.idx = index;
        it.ptr = newItem;
        return it;
    }

    else{ // finding item
        while(newItem != NULL){
            if(strcmp(key, newItem->key) == 0){
                it.t = t;
                it.idx = index;
                it.ptr = newItem;
                return it;
            }
            newItem = newItem->next;
        }

        if(newItem == NULL){
            struct htab_item *tmp = NULL; 
            newItem = malloc(sizeof(struct htab_item));
            newItem->key = malloc(sizeof(char) * (strlen(key)+1));
            
            if(newItem == NULL || newItem->key == NULL){
                fprintf(stderr,"%s", "Could not allocate memory\n");
                it.t = t;
                it.ptr = NULL;
                it.idx = index; // or -1
                return it;
            }
            newItem->next = NULL;
            memcpy(newItem->key, key, strlen(key) + 1);
            newItem->data = 0;

            tmp = t->array[index];

            while(tmp->next != NULL){
                tmp = tmp->next;
            }

            tmp->next = newItem;

            t->size++;

            it.t = t;
            it.idx = index;
            it.ptr = newItem;
            
        }
        return it;
    }
}