CROSS_COMPILE ?=
CC=$(CROSS_COMPILE)gcc

writer: writer.o
	$(CC) writer.o -o writer
writer.o: writer.c
	$(CC) -c writer.c -o writer.o
writer.c: 
	echo compiling

clean:
	-rm writer.o writer
