# Makefile for assignment 3
# 

CC_FLAGS = -Wall -g

proddonuts consdonuts : producer.o consumer.o assignment3.h sem_utils.o
	gcc $(CC_FLAGS) -o proddonuts producer.o sem_utils.o
	gcc $(CC_FLAGS) -o consdonuts consumer.o sem_utils.o

producer.o : producer.c assignment3.h
	gcc $(CC_FLAGS) -c producer.c

consumer.o : consumer.c assignment3.h
	gcc $(CC_FLAGS) -c consumer.c

sem_utils.o : sem_utils.c assignment3.h
	gcc $(CC_FLAGS) -c sem_utils.c

clean : 
	rm *.o proddonuts consdonuts
