/*
 * =====================================================================================
 *
 *       Filename:  htab_end.c
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

void htab_clear(htab_t *t){
 
    for (size_t i = 0; i < t->arr_size; i++){
        struct htab_item *current = t->array[i];
        struct htab_item *next = NULL;
        
         while (current != NULL){
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
        t->array[i] = NULL;
    }
    t->size = 0; 
    // wont free the whole table because the table structure has to stay alive
}