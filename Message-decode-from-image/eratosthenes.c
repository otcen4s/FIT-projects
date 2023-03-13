//===============================================================//
//File:         "eratosthenes.c"
//Prelozene 	gcc version 7.3.0
//Autor:        Matej Otcenas
//Login:        xotcen01
//Fakulta:      Fakulta informacnych technologii VUT v Brne
//Datum:        14.3.2019
//
//Popis modulu: -> vyuzitie algoritmu Erastosthenovo sito 
// 		-> (vid => https://www.smartickmethod.com/blog/math/operations-and-algebraic-thinking/divisibility/prime-numbers-sieve-eratosthenes/)
// 		-> iterujeme od 2 (0,1 nie su prvocisla) az po sqrt(velkost_pola) => viac zadanie
// 		-> pomocu bitovych poli zistujeme ci je pole[i] nastavene bitovo na 0
// 		-> ak je podmienka vyhodonotena ako true => nasli sme prvocislo a pomocou druheho cyklu nastavime vsetky jeho nasobky bitovo na 1
// 		-> tymto postupom efektivne "vypadnu" nasledujuce nasobky prvocisiel, ktore uz prvocislami nebudu
//===============================================================//

#include <stdio.h>
#include <math.h>
#include "bit_array.h"

void Eratosthenes(bit_array_t *pole){
	unsigned long velkost = bit_array_size(pole);
	unsigned long velkost_sqrt = (unsigned long) sqrt(velkost);
	
	
	for(unsigned long i = 2; i <= velkost_sqrt; i++){

		if(bit_array_getbit(pole, i) == 0){
			for(unsigned long j = 2; j*i < velkost; j++){
				bit_array_setbit(pole, j*i, 1);
			}
		}
	}
}

