/*
 * =====================================================================================
 *
 *       Filename:  htab_init.c
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

htab_t *htab_init(size_t n){    
    htab_t *table = malloc(sizeof(htab_t) + n * sizeof(struct htab_item));
    if (table == NULL){
        fprintf(stderr, "%s", "Could not allocate memory\n");
        return NULL;
    }

    table->size = 0;
    table->arr_size = n;
   
    for (size_t i = 0; i < n; i++){
        table->array[i] = NULL;
    }

    return table;
}
