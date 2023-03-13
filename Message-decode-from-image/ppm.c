//===============================================================//
//File:         "ppm.c"
//Prelozene: 	gcc version 7.3.0
//Autor:        Matej Otcenas
//Login:        xotcen01
//Fakulta:      Fakulta informacnych technologii VUT v Brne
//Datum:        14.3.2019
//
//Popis modulu: -> pokusi sa nacitat spravne rozhranie .ppm formatu obrazku zo suboru
// 		-> .ppm musi obsahovat "MAGIC NUMBER" => P6
// 		-> nasleduju hodnoty x,y (suradnice udavajuce rozmer x*y pixelov obrazku)
// 		-> potom nacita RGB format (indexova hodnota) => 255 (cislo udava rozmedzie farieb)
// 		-> dynamicky alokuje strukturu a vytvori jej priestor na heape o velkosti (x*y*3) => 3 == RGB
// 		-> nacita data farebnych zloziek zo suboru
//===============================================================//

#include "ppm.h"
#include <stdlib.h>
#include <stdio.h>
#include "error.h"


struct ppm *ppm_read(const char *filename){
	FILE *file;
	struct ppm *image = NULL;
	char P6 [2] = {0};
	unsigned x,y;
	int RGB_format , size;

	file = fopen(filename, "rb");

	if(file == NULL){
		warning_msg("Chybny format, nepodarilo sa otvorit subor %s\n", filename);
		return NULL;
	}
	
 	size = fscanf(file,"%c %c", &P6[0], &P6[1]);

	if(P6[0] != 'P' || P6[1] != '6' || size != 2){
		fclose(file);
		error_exit("Nacitany subor %s nie je definovany ako P6(magic number)\n", filename);
	}

	size = fscanf(file, "%u %u", &x, &y);

	if(size != 2){
		fclose(file);
		error_exit("Nespravny format velkosti suboru %s\n", filename);
	}

	size = fscanf(file," %d", &RGB_format);
	
	if(size != 1 || RGB_format != 255){
		fclose(file);
		error_exit("Format %d nesplnuje podmieky RGB(255)\n", RGB_format);
	}

	image = (struct ppm*)malloc(sizeof(struct ppm) + (x * y * 3));
	
	if(!image){
		fclose(file);
		error_exit("Nepodarilo sa alokovat pamat\n");
	}

	size = fread(image->data, sizeof(char), (3*x*y), file);
	
	if((unsigned)size != (3*x*y) || (feof(file)) != 0){
		fclose(file);
		ppm_free(image);
		error_exit("Nepodarilo sa nacitat data zo suboru %s\n", filename);
	}

	image->xsize = x;
	image->ysize = y;

	fclose(file);

	return image;
}

void ppm_free(struct ppm *image){
	free(image);
}
