#
#

VPATH = src:include:lib:build:tests

FLAGS = -Wall -g -I include
LIBS = -pthread

INCLUDE = $(include/*/*.h include/*.h)


build/bmproto.o : bmproto.c $(INCLUDES)
	gcc $(FLAGS) -c $< -o $@ $(LIBS)

build/binheap.o : binheap.c $(INCLUDES)
	gcc $(FLAGS) -c $< -o $@ $(LIBS)

build/mlist.o : mlist.c $(INCLUDES)
	gcc $(FLAGS) -c $< -o $@ $(LIBS)

build/mqueue.o : mqueue.c $(INCLUDES)
	gcc $(FLAGS) -c $< -o $@ $(LIBS)
 
	


