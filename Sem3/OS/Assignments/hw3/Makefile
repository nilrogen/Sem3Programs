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
PROGRAMS = bin/bm_server bin/nm bin/producer

all : $(BUILDDIR) $(BINDIR) $(OBJECTS) $(PROGRAMS)
	@echo Done

# executable file compilation
$(BINDIR)/bm_server : src/bm_server.c src/bm_server.h $(OBJECTS)
	@echo Compiling $<
	$(CC) $(FLAGS) $< $(OBJECTS) -o $@ $(LIBS)
	@echo

$(BINDIR)/nm : src/nm.c src/nm.h $(OBJECTS)
	@echo Compiling $<
	$(CC) $(FLAGS) $< $(OBJECTS) -o $@ $(LIBS)
	@echo

$(BINDIR)/producer : src/producer.c $(OBJECTS)
	@echo Compiling $<
	$(CC) $(FLAGS) $< $(OBJECTS) -o $@ $(LIBS)
	@echo

# Object file compilation
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
	rm -r build
	rm -r bin



