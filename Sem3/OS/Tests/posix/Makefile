FLAGS = -Wall -g
LIBS = -lrt

OBJF = shmsetup.o

dispatch : dispatch.c $(OBJF) header.h
	gcc $(FLAGS) -o dispatch dispatch.c $(OBJF) $(LIBS)

p1 : process1.c $(OBJF) header.h
	gcc $(FLAGS) -o p1 process1.c $(OBJF) $(LIBS)

shmsetup.o : shmsetup.c header.h
	gcc $(FLAGS) -c shmsetup.c $(LIBS)


clean :
	rm -f *.o
