PROGRAM = flp21-fun
FILES = src/*.hs test/* doc/* Makefile README 
SRC = src/*.hs
FLAGS = -Wall --make -o 

all: $(PROGRAM)

$(PROGRAM): $(SRC)
	ghc $(FLAGS) ${PROGRAM} $(SRC)

clean:
	rm src/*.o src/*.hi ${PROGRAM}

zip:
	zip flp-fun-xotcen01.zip ${FILES}