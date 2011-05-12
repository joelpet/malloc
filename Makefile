##################################################
## General configuration
## =====================

# Every Makefile should contain this line:
SHELL = /bin/sh

# Program for compiling C programs. 
CC = gcc

# Extra flags to give to the C preprocessor and programs that use it (the C and Fortran compilers). 
CFLAGS = 

# Default plus extra flags for C preprocessor and compiler.
all_cflags = $(CFLAGS) -Wall -Wextra -g -ansi -DSTRATEGY=2


##################################################
## Setup files variables
## =====================

# Source files to compile and link together
srcs = malloc.c tstalgorithms.c tstcrash_complex.c tstcrash_simple.c \
	   tstextreme.c tstmalloc.c tstmemory.c tstrealloc.c tstmerge.c

# Deduce object files from source files
objs = $(srcs:.c=.o)

# Executables
execs = t0 t1 t2 t3 t4 t5 t6 t7


##################################################
## Ordinary targets
## ================

# http://www.gnu.org/software/make/manual/make.html#Phony-Targets
# These are not the name of files that will be created by their recipes.
.PHONY: all clean

all: $(execs)

t0: tstmerge.o malloc.o
	$(CC) $(all_cflags) -o $@ $<
                               
t1: tstalgorithms.o  malloc.o
	$(CC) $(all_cflags) -o $@ $<
                              
t2: tstextreme.o malloc.o
	$(CC) $(all_cflags) -o $@ $<
                             
t3: tstmalloc.o malloc.o
	$(CC) $(all_cflags) -o $@ $<
                            
t4: tstmemory.o malloc.o
	$(CC) $(all_cflags) -o $@ $<
                           
t5: tstrealloc.o malloc.o
	$(CC) $(all_cflags) -o $@ $<
                          
t6: tstcrash_simple.o malloc.o
	$(CC) $(all_cflags) -o $@ $<
                         
t7: tstcrash_complex.o malloc.o
	$(CC) $(all_cflags) -o $@ $<

%.o: %.c
	$(CC) -c $(all_cflags) $< -o $@

clean:
	-rm -f *.o $(execs) core

