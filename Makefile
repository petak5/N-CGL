CC=gcc
LIBS=-lcurses
CFLAGS=-std=c99 -Wall -Wextra -Werror

all: main

main: main.o
	clear
	$(CC) $(CFLAGS) $(LIBS) main.o -o main

main.o: main.c
	clear
	$(CC) $(CFLAGS) main.c -c -o main.o

run: main
	./main

clean: main main.o
	rm main main.o
