# Michael Gorlin
# Makefile for hw3 
CC = gcc

FLAGS = -Wall -g -I include
LIBS = -pthread

BUILDDIR = build
BINDIR = bin

INCLUDES = $(shell find include/ -name *.h)
SOURCES = $(shell find lib/ -name *.c)
OBJECTS = $(SOURCES:lib/%.c=$(BUILDDIR)/%.o)
PROGRAMS = bin/bm_server bin/nm

all : $(BUILDDIR) $(BINDIR) $(OBJECTS) $(PROGRAMS)
	@echo Done

bin/bm_server : src/bm_server.c src/bm_server.h $(OBJECTS)
	$(CC) $(FLAGS) $< -o $@ $(LIBS)

bin/nm : src/nm.c src/nm.h $(OBJECTS)
	$(CC) $(FLAGS) $< -o $@ $(LIBS)

$(BUILDDIR)/%.o : lib/%.c $(INCLUDE) 
	@echo "Compiling: $<"
	$(CC) $(FLAGS) -c $< -o $@ $(LIBS)
	@echo

$(BUILDDIR) :
	@echo Making Build Directory 
	mkdir $(BUILDDIR)
	@echo
$(BINDIR) : 
	@echo Making Bin Directory
	mkdir $(BINDIR)
	@echo 

.PHONY: clean
clean : 
	rm build/*.o



