CFLAGS=-g -lm -Wall -O3 -std=gnu99
CC=gcc
barnes_hut:
	$(CC) barnes_hut.c $(CFLAGS) -o barnes-hut
clean:
	rm -rf *~ barnes_hut
