/*
 * =====================================================================================
 *
 *       Filename:  io.c
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "io.h"


int get_word(char *s, int max, FILE *f){
    int position = 0;
    char c;
    bool isSpace = false;

    while(((c = fgetc(f)) != EOF) && (position != max - 1)){
        if(!isspace(c)){
            s[position] = c;
            position++;    
        } 
        else{
            if(position > 0){
                isSpace = true;
                break;
            }
        }
    }
    s[position] = '\0';

    // read rest of word
    if (position == max - 1){
        bool end_read = false;
        do{
			c = getc(f);
			if (isspace(c)) {
				end_read = true;
			}
        } while (!end_read);
        return 42;
    }    

    if((c =! EOF) && (isSpace == false)){
        return EOF;
    }
    else if(strlen(s) == 0){
        return EOF;
    }
    else if(c == EOF){
        return EOF;
    }

    return strlen(s);
    
}