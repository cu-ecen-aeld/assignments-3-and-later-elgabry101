CROSS_COMPILE ?=
CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall

# Default target
all: aesdsocket

aesdsocket: aesdsocket.o
	$(CC) $(CFLAGS) aesdsocket.o -o aesdsocket

aesdsocket.o: aesdsocket.c
	$(CC) $(CFLAGS) -c aesdsocket.c -o aesdsocket.o

clean:
	rm -f aesdsocket.o aesdsocket
