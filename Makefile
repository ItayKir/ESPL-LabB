CC = gcc
CFLAGS = -Wall -g

all: bubblesort hexaPrint virusDetector

bubblesort: lab3_bubblesort.c
	$(CC) $(CFLAGS) -o bubblesort lab3_bubblesort.c

hexaPrint: hexaPrint.c
	$(CC) $(CFLAGS) -o hexaPrint hexaPrint.c

virusDetector: virusDetector.c
	$(CC) $(CFLAGS) -o virusDetector virusDetector.c

clean:
	rm -f bubblesort hexaPrint virusDetector