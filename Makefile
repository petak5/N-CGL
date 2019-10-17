CC=gcc
LIBS=-lcurses
CFLAGS=-std=c99 -Wall -Wextra -Werror -g

all: main

main: main.o
	$(CC) $(CFLAGS) $(LIBS) $^ -o main

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

run: main
	./main

clean:
	rm main main.o
