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

htab_iterator_t htab_end(const htab_t * t){
     htab_iterator_t end;
     struct htab_item *item;
     for (size_t i = htab_bucket_count(t); i > 0; i--){
         if (t->array[i - 1] != NULL){
             item = t->array[i - 1];
             while (item != NULL){
                 item = item->next;
             }
            end.t = t;
            end.idx = i;
            end.ptr = item;
            return end;
         }
     }

     end.t = t;
     end.ptr = NULL;
     end.idx = 0;

     return end;
}