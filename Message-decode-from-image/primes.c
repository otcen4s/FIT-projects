//===============================================================//
//File:         "primes.c"
//Prelozene 	gcc version 7.3.0
//Autor:        Matej Otcenas
//Login:        xotcen01
//Fakulta:      Fakulta informacnych technologii VUT v Brne
//Datum:        14.3.2019
//
//Popis modulu: -> hlada prvocisla v poli o velkosti (123-milionov)
// 		-> technika vyhladavania (Eratosthenove sito) pomocou bitoveho pola (viac -> "bit_array.h")
// 		-> vypis poslednych 10 prvocisel na stdout
//===============================================================//


#include <stdio.h>
#include <math.h>
#include "bit_array.h"
#include "error.h"

int main(){
	
	bit_array_create(pole, 123000000L);
	Eratosthenes(pole);
	
	unsigned long pocet = 0;
	unsigned long vypis_pola[10];

	for(unsigned long i = 123000000L - 1; i > 0 ; i--){
		
		if(bit_array_getbit(pole, i) == 0){
			
		 	if(pocet > 10) break;
			
			vypis_pola[pocet] = i;
			
			pocet++;
			
		}
	}

  	for(unsigned long i = 10; i < pocet; i--){
		printf("%lu\n", vypis_pola[i]);
	}
	return 0;
}
