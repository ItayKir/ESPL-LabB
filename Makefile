CC = gcc
CFLAGS = -Wall -g

all: bubblesort hexaPrint

bubblesort: lab3_bubblesort.c
	$(CC) $(CFLAGS) -o bubblesort lab3_bubblesort.c

hexaPrint: hexaPrint.c
	$(CC) $(CFLAGS) -o hexaPrint hexaPrint.c

clean:
	rm -f bubblesort hexaPrint