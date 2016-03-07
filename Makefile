# choose your compiler
CC	= gcc -g
	#CC=gcc
	#CC=gcc -Wall
NO_LINK	= -c
	#LIBS	= -L. -lbulitin


all: builtin mysh


builtin:
	cd builtin && $(MAKE)


mysh: sh.o get_path.o parseCommandLine.o main.c 
	cd builtin && $(MAKE)
	$(CC) $^ builtin/builtin.o -o $@
#	$(CC) main.c sh.o get_path.o -o mysh
#	$(CC) main.c sh.o get_path.o bash_getcwd.o -o mysh

sh.o: sh.c sh.h
	$(CC) $(NO_LINK) sh.c

get_path.o: get_path.c get_path.h
	$(CC) $(NO_LINK) get_path.c

parseCommandLine.o: parseCommandLine.c parseCommandLine.h
	$(CC) $(NO_LINK) parseCommandLine.c


clean:
	rm -rf sh.o get_path.o parseCommandLine.o mysh
	cd builtin && $(MAKE) clean

