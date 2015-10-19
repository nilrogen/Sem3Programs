
FLAGS = -Wall -g
LIBS = -pthread 

OBJF = evseq.o

test : assignment2.c $(OBJF) assignment2.h 
	gcc $(FLAGS) -o test assignment2.c $(OBJF) $(LIBS)

evseq.o : evseq.c evseq.h assignment2.h 
	gcc $(FLAGS) -c evseq.c $(LIBS)

clean : 
	rm *.o test

