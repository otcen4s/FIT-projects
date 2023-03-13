//===============================================================//
//File:         "steg-decode.c"
//Prelozene: 	gcc version 7.3.0
//Autor:        Matej Otcenas
//Login:        xotcen01
//Fakulta:      Fakulta informacnych technologii VUT v Brne
//Datum:        14.3.2019
//
//Popis modulu: -> dekoduje "tajnu" spravu zasiforvanu v obrazku formatu .ppm pomocu steganografie
// 		-> vyuziva algoritmus hladania prvocisel tzv. Eratosthenova siet a pri uspesnom najdeni
// 		-> prvocisla vyberie jeho LSB a na zaklade hodnot 1/0 vysklada 8-bit char ktoreho hodnota
// 		-> predstavuje v ASCII (UTF-8) jeden znak
// 		-> vypis na stdout
//===============================================================//

#include "eratosthenes.h"
#include "bit_array.h"
#include "ppm.h"
#include <stdio.h>
#include "error.h"

unsigned long get_bit_foo(char msg [], unsigned index){
	return (msg[1 + (index / SIZE)] & (unsigned long)1 << (index % SIZE) ? 1 : 0);
}

int main(int argc, char* argv[]){
	
	if(argc < 2) error_exit("Subor sa nepodarilo nacitat\n");

 	char *file;
	file = argv[1];

        struct ppm *image;
        image = ppm_read(file);
	
	if(image == NULL){
		ppm_free(image);
		return -1;
	}	

	unsigned long MAX = (image->xsize) * (image->ysize) * 3;

	bit_array_alloc(pole, MAX);
  	Eratosthenes(pole);
	
	unsigned long index = 19;
	unsigned long bits_counter = 0;
	char message [2] = {MAX, 0};

	for(unsigned i = index; i < MAX; i++){
		
		  if(bit_array_getbit(pole,i) == 0){
			 
		  	if(get_bit_foo(&image->data[i], 0)){	
		 		message[1] |= 1 << (bits_counter);
			}
 			else {
				message[1] &= ~(1 << (bits_counter));
			}

			bits_counter++;
			
			if(bits_counter == CHAR_BIT){
				if(message[1] == 0) break;

				printf("%c", message[1]);
				
				bits_counter = 0;

				message[1] = 0;
			}
		}
	}

	ppm_free(image);
	bit_array_free(pole);

	if(message[1] != 0){
		error_exit("Kod spravy nebol ukonceny spravne\n");
	}

	printf("\n");

	return 0;
}

