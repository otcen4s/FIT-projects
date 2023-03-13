//===============================================================//
//File:     "error.c"
//Autor:    Matej Otcenas
//Login:    xotcen01
//Fakulta:  Fakulta informacnych technologii VUT v Brne
//Datum:    19.3.2019
//===============================================================//

#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void warning_msg(const char *fmt, ...)
{
	va_list args;

   	va_start(args, fmt);

   	fprintf(stderr,"CHYBA: ");
   	vfprintf(stderr, fmt, args);

   	va_end(args);
}

void error_exit(const char *fmt, ...)
{
	va_list args;

   	va_start(args, fmt);

	fprintf(stderr, "CHYBA: ");
  	vfprintf(stderr, fmt, args);
   	
	va_end(args);
   	
	exit(1);
}
