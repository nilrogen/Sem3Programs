
FLAGS = -Wall -g -Wextra
LIBS = -pthread 

OBJF = evseq.o

program : assignment2.c $(OBJF) assignment2.h 
	gcc $(FLAGS) -o program assignment2.c $(OBJF) $(LIBS)

evseq.o : evseq.c evseq.h assignment2.h 
	gcc $(FLAGS) -c evseq.c $(LIBS)

clean :
	rm *.o program

