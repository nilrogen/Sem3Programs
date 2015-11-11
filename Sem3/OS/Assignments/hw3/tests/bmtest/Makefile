BMPROTO=../../build/bmproto.o
INCLUDE=../../include

SRC=server.c prod.c
OBJ=build/producer.o

FLAGS=-g -Wall -I $(INCLUDE)
LIBS=-pthread


VPATH = ../../include:../../build
server prod : server.c $(BMPROTO) $(OBJ) bmtest.h
	gcc $(FLAGS) -o server server.c $(BMPROTO) $(OBJ) $(LIBS)
	cp server prod


build/producer.o : producer.c bmtest.h
	gcc $(FLAGS) -o $@ -c producer.c $(BMPROTO) $(LIBS)
