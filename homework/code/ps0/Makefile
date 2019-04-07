CFLAGS = -Wall -Werror -Wpedantic -std=gnu99 -O0 -g
CC = gcc

OBJECTS = main.o bst.o

default: run-tests

run-tests: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c bst.h

bst.o: bst.c bst.h

clean:
	rm -f run-tests *.o *~
