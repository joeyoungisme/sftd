CC=gcc
CFLAGS=-g -Wall -O3
LDLIBS=
OBJECTS=err.o pdu.o cmd.o

all: server client

server: $(OBJECTS) sftd.o
	$(CC) $(LDLIBS) -o sftd $(OBJECTS) sftd.o

client: $(OBJECTS) sftc.o
	$(CC) $(LDLIBS) -o sftc $(OBJECTS) sftc.o

sftc.o: sftc.c
	$(CC) $(CFLAGS) -c sftc.c

sftd.o: sftd.c
	$(CC) $(CFLAGS) -c sftd.c

err.o: err.c
	$(CC) $(CFLAGS) -c err.c

pdu.o: pdu.c
	$(CC) $(CFLAGS) -c pdu.c

cmd.o: cmd.c
	$(CC) $(CFLAGS) -c cmd.c

clean:
	rm *.o sftd sftc
