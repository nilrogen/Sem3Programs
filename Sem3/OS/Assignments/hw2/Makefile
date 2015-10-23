# Author: Michael Gorlin
#

FLAGS = -Wall -g
LIBS = -pthread 

OBJF = evseq.o

procscope, threadscope: assignment2.c $(OBJF) assignment2.h 
	gcc $(FLAGS) -o procscope assignment2.c $(OBJF) $(LIBS)
	gcc $(FLAGS) -o threadscope assignment2.c $(OBJF) $(LIBS)

evseq.o : evseq.c evseq.h assignment2.h 
	gcc $(FLAGS) -c evseq.c $(LIBS)

clean :
	rm *.o procscope threadscope
	if [ -e fulloutput ]; then rm -r fulloutput/; mkdir fulloutput/; fi;
	rm cons*
