//===============================================================//
//File:     "error.h"
//Autor:    Matej Otcenas
//Login:    xotcen01
//Fakulta:  Fakulta informacnych technologii VUT v Brne
//Datum:    19.3.2019
//===============================================================//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void warning_msg(const char *fmt, ...);
void error_exit(const char *fmt, ...);
