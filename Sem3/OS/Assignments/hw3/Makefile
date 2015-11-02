#
#

VPATH = src:include:lib:build:tests

FLAGS = -Wall -g -I include
LIBS = -pthread

INCLUDE = $(include/*/*.h include/*.h)

build/%.o : %.c $(INCLUDES)
	@echo $< = $@

build/mlist.o : mlist.c $(INCLUDES)
	gcc $(FLAGS) -c $< -o $@ $(LIBS)

build/mqueue.o : mqueue.c $(INCLUDES)
	gcc $(FLAGS) -c $< -o $@ $(LIBS)
 
	


