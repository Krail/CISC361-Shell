# choose your compiler
CC      = gcc -g
NO_LINK = -c


all: builtin.o


builtin.o: alias.c alias.h
	$(CC) $(NO_LINK) alias.c -o $@


clean:
	rm -rf alias.o builtin.o

