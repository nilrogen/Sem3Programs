FLAGS = -Wall -g
LIBS = -lrt -pthread

OBJF = shmsetup.o semsetup.o

producer consumer : producer.c consumer.c $(OBJF) assignment1.h
	gcc $(FLAGS) -o producer producer.c $(OBJF) $(LIBS)
	gcc $(FLAGS) -o consumer consumer.c $(OBJF) $(LIBS)

shmsetup.o : shmsetup.c assignment1.h
	gcc $(FLAGS) -c shmsetup.c $(LIBS)

semsetup.o : semsetup.c assignment1.h
	gcc $(FLAGS) -c semsetup.c $(LIBS)

clean :
	rm -f *.o
	
