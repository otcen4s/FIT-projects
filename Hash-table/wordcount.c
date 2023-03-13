/*
 * =====================================================================================
 *
 *       Filename:  wordcount.c
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
#include <stdio.h>
#include <stdlib.h>
#include "io.h"

#define MAX 127

int main(){
    FILE *fp = stdin;
    int data;
    int ret_value;
    bool alert = false;
    char key[MAX];

    /** This size of table was chosen due to testing on sequention of billion numbers that causes the fastest output approximately
    * 3.5 seconds which is obvious beacuse every number was uniq so we had to always add a new item but if we would add one zero up
    * it would be useless for this type of input because our hash function would be generating the same indexes but of course one zero
    * down would affect the time of the output due to collisions that would be causing creation of longer linked lists. Anyway it might
    * take quiet a lot of memory to allocate so if my input would be containing some different not uniq data it would be wise to reconsider
    * the initial size of table.
    */
    htab_t *t = htab_init(1000000);
    if(t == NULL) return -1;
    
    htab_iterator_t it; 

    while((ret_value = get_word(key, MAX, fp)) != EOF){
        if(ret_value == 42 && (!alert)){
            alert = true;
            fprintf(stderr, "%s", "Length of word over [127 B] was reached, words will be cut\n");
        }

        it = htab_lookup_add(t, key);
        if(it.ptr == NULL) return -1;

        data = htab_iterator_get_value(it) + 1;
        htab_iterator_set_value(it, data);
    }

    if(ret_value == EOF && htab_size(t) == 0){
        fprintf(stderr,"%s", "No input given, please run this program with some data\n");
        htab_free(t);
        return -1;
    }

    #ifdef TEST
        htab_t *new_t;

        new_t = htab_move(htab_bucket_count(t), t);
        if(new_t == NULL) return -1;

        htab_iterator_t begin_n = htab_begin(new_t);
        htab_iterator_t end_n = htab_end(new_t);

        while(!(htab_iterator_equal(begin_n, end_n))){
            printf("%s %d\n",htab_iterator_get_key(begin_n) ,htab_iterator_get_value(begin_n));
            begin_n = htab_iterator_next(begin_n);
        } 
        free(t);
        htab_free(new_t);

    #else 
        htab_iterator_t begin = htab_begin(t);
        htab_iterator_t end = htab_end(t);

        while(!(htab_iterator_equal(begin, end))){
            printf("%s %d\n",htab_iterator_get_key(begin) ,htab_iterator_get_value(begin));
            begin = htab_iterator_next(begin);
        }
        htab_free(t);

    #endif

   
    return 0;
}