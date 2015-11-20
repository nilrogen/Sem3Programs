BMPROTO=../../build/bmproto.o
INCLUDE=../../include

SRC=server.c prod.c
OBJ=build/client.o

FLAGS=-g -Wall -I $(INCLUDE)
LIBS=-pthread


VPATH = ../../include:../../build
server prod : server.c $(BMPROTO) $(OBJ) bmtest.h
	gcc $(FLAGS) -o server server.c $(BMPROTO) $(OBJ) $(LIBS)
	cp server prod
	cp server cons


build/client.o : client.c bmtest.h
	gcc $(FLAGS) -o $@ -c client.c $(BMPROTO) $(LIBS)
