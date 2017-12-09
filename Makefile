CC=gcc
CFLAGS=-pipe
INC=

all: pipeSimulator
	$(CC) $(CFLAGS) pipeSimulator.o -o pipeSimulator

pipeSimulator: pipeSimulator.c
	$(CC) $(CFLAGS) -c pipeSimulator.c

dist:
	tar -cvjf dist.tbz2 pipeSimulator.c Makefile

clean:
	rm  pipeSimulator.o pipeSimulator
