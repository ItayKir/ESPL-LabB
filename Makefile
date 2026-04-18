CC = gcc
CFLAGS = -Wall -g

all: bubblesort

bubblesort: lab3_bubblesort.c
	$(CC) $(CFLAGS) -o bubblesort lab3_bubblesort.c

clean:
	rm -f bubblesort