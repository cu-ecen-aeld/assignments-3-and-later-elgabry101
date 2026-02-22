CROSS_COMPILE ?=
CC=$(CROSS_COMPILE)gcc
CFLAGS=-Wall

# Default target
all: writer

writer: writer.o
    $(CC) $(CFLAGS) writer.o -o writer

writer.o: writer.c
    $(CC) $(CFLAGS) -c writer.c -o writer.o

clean:
    rm -f writer.o writer 

