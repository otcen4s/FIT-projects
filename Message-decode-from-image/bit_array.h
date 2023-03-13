//===============================================================//
//File: 	"bit_array.h"
//Prelozene: 	gcc version 7.3.0
//Autor: 	Matej Otcenas
//Login: 	xotcen01
//Fakulta: 	Fakulta informacnych technologii VUT v Brne
//Datum: 	14.3.2019
//===============================================================//

#include <assert.h>
#include "error.h"
#include <limits.h>
#include <stdio.h>


#define static_assert _Static_assert

typedef unsigned long bit_array_t;

void Eratosthenes(bit_array_t* pole);

#define SIZE (sizeof(unsigned long) * CHAR_BIT)


/* bitove makra ktore vytvoria bitove pole */

#define bit_array_create(meno_pola, velkost) \
		static_assert((velkost) > 0, "Presiahnuty limit pola"); \
		unsigned long (meno_pola) [1 + ((velkost)/SIZE) + (((velkost) % SIZE) > 0 ? 1 : 0)] = {(velkost), 0} 


#define bit_array_alloc(meno_pola, velkost) \
		assert((velkost) > 0 ); \
	   	bit_array_t *meno_pola = calloc(1 + ((velkost)/SIZE) + (((velkost) % SIZE) > 0 ? 1 : 0), sizeof(bit_array_t)); \
		meno_pola[0] = (velkost); \
		if(meno_pola == NULL) error_exit("bit_array_alloc: Chyba alokacie pamati") 


#define bit_array_free(meno_pola) free(meno_pola)

////////////////////////////////////////////////////////////////////////////

#ifdef USE_INLINE

inline unsigned long bit_array_size(bit_array_t *meno_pola){ return meno_pola[0]; }

#else

#define bit_array_size(meno_pola) meno_pola[0]

#endif

////////////////////////////////////////////////////////////////////////////

#ifdef USE_INLINE

inline void bit_array_setbit(bit_array_t *meno_pola, unsigned long index, unsigned long vyraz){
	if(index > bit_array_size(meno_pola) - 1){
		error_exit("Index [%lu] v bitovom poli presahuje limit 0..%lu!(set_bit_error)\n", index, bit_array_size(meno_pola) -1);
	}
	else{
		(vyraz) ? (meno_pola[1 + (index / SIZE)] |= (unsigned long)1 << (index % SIZE)) : (meno_pola[1 + (index / SIZE)] &= ~((unsigned long)1 << (index % SIZE)));	
	}
}

#else 

#define bit_array_setbit(meno_pola, index, vyraz) \
		do { \
			if((index) > bit_array_size(meno_pola)) { \
				error_exit("Index [%lu] v bitovom poli presahuje limit 0..%lu!(set_bit_error)\n", (unsigned long)(index), (unsigned long)bit_array_size(meno_pola)-1); \
			} \
			else{ \
				(vyraz) ? \
				(meno_pola[1 + ((index)/ SIZE) ] |= (unsigned long)1 << ((index) % SIZE)) : \
				(meno_pola[1 + (index)/ SIZE ] &= ~((unsigned long)1 << ((index) % SIZE))); \
			} \
		} \
		while(0)
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef USE_INLINE

inline unsigned long bit_array_getbit(bit_array_t *meno_pola, unsigned long index){
	if(index > bit_array_size(meno_pola) - 1){
		error_exit("Index [%lu] v bitovom poli presahuje limit 0..%lu!(get_bit_error)\n", index, bit_array_size(meno_pola) - 1);
		return -1;
	}
	else{
		return (meno_pola[1 + (index / SIZE)] & (unsigned long)1 << (index % SIZE) ? 1 : 0);
	}
}

#else

#define bit_array_getbit(meno_pola, index) ((index) > bit_array_size(meno_pola)) ?\
       		error_exit("Index [%lu] v bitovom poli presahuje limit 0..%lu!(get_bit_error)\n", (unsigned long)(index), (unsigned long)bit_array_size(meno_pola)-1), -1 : \
		((meno_pola[1 + ((index) / SIZE)] & (unsigned long)1 << ((index) % SIZE)) ? 1 : 0 )
#endif 

/////////////////////////////////////////////////////////////////////////////
