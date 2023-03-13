CC=gcc

CFLAGS=-std=c99 -pedantic -Wall -Wextra -g

CPPFLAGS=-std=c++11 -pedantic -Wall -Wextra

OBJECTS=htab_begin.o htab_bucket_count.o htab_clear.o htab_end.o htab_free.o htab_hash_function.o htab_init.o htab_iterator_get_key.o htab_iterator_get_value.o htab_iterator_next.o htab_iterator_set_value.o htab_lookup_add.o htab_size.o htab_move.o

DYNAMIC_OBJECTS=htab_init-dynamic.o htab_hash_function-dynamic.o htab_size-dynamic.o htab_lookup_add-dynamic.o htab_iterator_set_value-dynamic.o htab_iterator_next-dynamic.o htab_iterator_get_value-dynamic.o htab_iterator_get_key-dynamic.o htab_free-dynamic.o htab_end-dynamic.o htab_clear-dynamic.o htab_bucket_count-dynamic.o htab_begin-dynamic.o htab_move-dynamic.o

MODULES=tail.c tail2.cc io.c wordcount.c htab_begin.c htab_bucket_count.c htab_clear.c htab_end.c htab_free.c htab_hash_function.c htab_init.c htab_iterator_get_key.c htab_iterator_get_value.c htab_iterator_next.c htab_iterator_set_value.c htab_lookup_add.c htab_size.c htab_move.c 

HEADERS= htab.h private_htab_s.h io.h

####################### ALL ######################
all: tail tail2 wordcount wordcount-dynamic
##################################################

###################### TAIL ######################
tail: tail.c
	$(CC) $(CFLAGS) tail.c -o tail

tail2: tail2.cc
	g++ $(CPPFLAGS) tail2.cc -o tail2 
##################################################

################## STATIC OBJECTS ################

htab_init.o: htab_init.c
	$(CC) $(CFLAGS) -c htab_init.c

########## TO RUN PROGRAM WITH DIFFERENT HASH FUNCTION ADD -DHASHTEST INTO THIS OBJECT ##########
htab_hash_function.o: htab_hash_function.c
	$(CC) $(CFLAGS) -c htab_hash_function.c

htab_size.o: htab_size.c
	$(CC) $(CFLAGS) -c htab_size.c

########## TO RUN PROGRAM WITH DIFFERENT HASH FUNCTION ADD -DHASHTEST INTO THIS OBJECT ##########
htab_lookup_add.o: htab_lookup_add.c
	$(CC) $(CFLAGS) -c htab_lookup_add.c

htab_iterator_set_value.o: htab_iterator_set_value.c
	$(CC) $(CFLAGS) -c htab_iterator_set_value.c

htab_iterator_next.o: htab_iterator_next.c
	$(CC) $(CFLAGS) -c htab_iterator_next.c

htab_iterator_get_value.o: htab_iterator_get_value.c
	$(CC) $(CFLAGS) -c htab_iterator_get_value.c

htab_iterator_get_key.o: htab_iterator_get_key.c
	$(CC) $(CFLAGS) -c htab_iterator_get_key.c

htab_free.o: htab_free.c
	$(CC) $(CFLAGS) -c htab_free.c

htab_end.o: htab_end.c
	$(CC) $(CFLAGS) -c htab_end.c

htab_clear.o: htab_clear.c
	$(CC) $(CFLAGS) -c htab_clear.c

htab_bucket_count.o: htab_bucket_count.c
	$(CC) $(CFLAGS) -c htab_bucket_count.c

htab_begin.o: htab_begin.c
	$(CC) $(CFLAGS) -c htab_begin.c

htab_move.o: htab_move.c
	$(CC) $(CFLAGS) -c htab_move.c

####################################################

##################### LIBRARIES ####################
libhtab.a: $(OBJECTS)
	ar rcs $@ $^

libhtab.so: $(DYNAMIC_OBJECTS)
	$(CC) $(CFLAGS) -fpic -shared $^ -o $@  
####################################################

####################### TO RUN THIS PROGRAM WITH HTAB_MOVE FUNCTION COMPILE IT WITH -DTEST #########################
wordcount: wordcount.c htab.h private_htab_s.h io.h io.c libhtab.a 
	$(CC) $(CFLAGS) $^ -o $@

################### DYNAMIC OBJECTS ################
htab_init-dynamic.o: htab_init.c
	$(CC) $(CFLAGS) -fpic -c htab_init.c -o $@

htab_hash_function-dynamic.o: htab_hash_function.c
	$(CC) $(CFLAGS) -fpic -c htab_hash_function.c -o $@

htab_size-dynamic.o: htab_size.c
	$(CC) $(CFLAGS) -fpic -c htab_size.c -o $@

htab_lookup_add-dynamic.o: htab_lookup_add.c
	$(CC) $(CFLAGS) -fpic -c htab_lookup_add.c -o $@

htab_iterator_set_value-dynamic.o: htab_iterator_set_value.c
	$(CC) $(CFLAGS) -fpic -c htab_iterator_set_value.c -o $@

htab_iterator_next-dynamic.o: htab_iterator_next.c
	$(CC) $(CFLAGS) -fpic -c htab_iterator_next.c -o $@

htab_iterator_get_value-dynamic.o: htab_iterator_get_value.c
	$(CC) $(CFLAGS) -fpic -c htab_iterator_get_value.c -o $@

htab_iterator_get_key-dynamic.o: htab_iterator_get_key.c
	$(CC) $(CFLAGS) -fpic -c htab_iterator_get_key.c -o $@

htab_free-dynamic.o: htab_free.c
	$(CC) $(CFLAGS) -fpic -c htab_free.c -o $@

htab_end-dynamic.o: htab_end.c
	$(CC) $(CFLAGS) -fpic -c htab_end.c -o $@

htab_clear-dynamic.o: htab_clear.c
	$(CC) $(CFLAGS) -fpic -c htab_clear.c -o $@

htab_bucket_count-dynamic.o: htab_bucket_count.c
	$(CC) $(CFLAGS) -fpic -c htab_bucket_count.c -o $@

htab_begin-dynamic.o: htab_begin.c
	$(CC) $(CFLAGS) -fpic -c htab_begin.c -o $@

htab_move-dynamic.o: htab_move.c
	$(CC) $(CFLAGS) -fpic -c htab_move.c -o $@

####################### TO RUN THIS PROGRAM WITH HTAB_MOVE FUNCTION COMPILE IT WITH -DTEST #########################
wordcount-dynamic: wordcount.c htab.h private_htab_s.h io.h io.c libhtab.so
	$(CC) -fpic $(CFLAGS) $^ -o $@ 

clean-all:
	rm -f tail tail2 wordcount wordcount-dynamic *.o *.a *.so xotcen01.zip

clean-lib:
	rm -f libhtab.a libhtab.so

clean: 
	rm -f tail tail2 wordcount wordcount-dynamic *.o

zip: $(MODULES) $(HEADERS)
	zip xotcen01.zip *.c *.cc *.h Makefile 
