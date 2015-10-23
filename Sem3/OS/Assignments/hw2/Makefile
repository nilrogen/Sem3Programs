# Author: Michael Gorlin
#

FLAGS = -Wall -g
LIBS = -pthread 

OBJF = evseq.o

processscope, systemscope: assignment2.c $(OBJF) assignment2.h 
	gcc $(FLAGS) -o processscope assignment2.c $(OBJF) $(LIBS)
	gcc $(FLAGS) -o systemscope assignment2.c $(OBJF) $(LIBS)

evseq.o : evseq.c evseq.h assignment2.h 
	gcc $(FLAGS) -c evseq.c $(LIBS)

clean :
	rm *.o processscope systemscope
	if [ -e fulloutput ]; then rm -r fulloutput/; mkdir fulloutput/; fi;
	rm cons
