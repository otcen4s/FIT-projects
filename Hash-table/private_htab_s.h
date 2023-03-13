/*
 * =====================================================================================
 *
 *       Filename:  private_htab_s.h
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

#ifndef __HTABLE_PRIVATE_H__
#define __HTABLE_PRIVATE_H__

#include <stdio.h>

struct htab_item{
    char *key;
    int data;
    struct htab_item *next;
};

struct htab{
    size_t size;
    size_t arr_size;
    struct htab_item *array[]; 
};

#endif