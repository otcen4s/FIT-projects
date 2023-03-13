/*
 * =====================================================================================
 *
 *       Filename:  htab_free.c
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

void htab_free(htab_t * t){
    htab_clear(t);
    free(t);
    t = NULL;   
}