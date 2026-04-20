CC = gcc
CFLAGS = -Wall -g

all: AntiVirus

AntiVirus: AntiVirus.c
	$(CC) $(CFLAGS) -o AntiVirus AntiVirus.c

clean:
	rm -f AntiVirus