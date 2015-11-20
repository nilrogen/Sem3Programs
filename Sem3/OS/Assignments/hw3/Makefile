# Michael Gorlin
# Makefile for hw3 

FLAGS = -Wall -g -I include
LIBS = -pthread

BUILDDIR = build
BINDIR = bin

INCLUDES = $(shell find include/ -name *.h)
SOURCES = $(shell find lib/ -name *.c)
OBJECTS = $(SOURCES:lib/%.c=$(BUILDDIR)/%.o)

all : $(BUILDDIR) $(OBJECTS)
	@echo Done

$(BUILDDIR) :
	@echo Making Build Directory 
	mkdir $(BUILDDIR)
	@echo

$(BUILDDIR)/%.o : lib/%.c $(INCLUDE) 
	@echo "Compiling: $<"
	gcc $(FLAGS) -c $< -o $@ $(LIBS)
	@echo

.PHONY: clean
clean : 
	rm build/*.o



