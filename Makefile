CC=gcc
CFLAGS=-std=c99
RM=rm -rf

all:
	$(CC) $(CFLAGS) diceroll.c -o diceroll

clean:
	$(RM) diceroll

.PHONY: all clean
