# The defaults here are for gcc.
CC=gcc
CFLAGS=-Wall -O3 -ansi
#CFLAGS=-g -Wall -ansi
RM=rm -f

saucy : saucy_test.o saucy.o
	$(CC) $(CFLAGS) -o  $@ saucy_test.o saucy.o

# Header dependencies
saucy_test.o : saucy.h
saucy.o : saucy.h saucy_int.h

# Useful fake targets
clean : ; $(RM) saucy *.o
