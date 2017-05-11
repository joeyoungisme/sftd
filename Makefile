CC=gcc
CFLAGS=-g -Wall -O3
LDLIBS=
OBJECTS=err.o pdu.o cmd.o sfts.o sftc.o sft.o

all: server client

server: $(OBJECTS)
	$(CC) $(CFLAGS) -c sft_server.c
	$(CC) $(LDLIBS) -o sft_server $(OBJECTS) sft_server.o

client: $(OBJECTS) 
	$(CC) $(CFLAGS) -c sft_client.c
	$(CC) $(LDLIBS) -o sft_client $(OBJECTS) sft_client.o

obj: $(OBJECTS) sft.o

sft.o: sft.c
	$(CC) $(CFLAGS) -c sft.c

sftc.o: sftc.c
	$(CC) $(CFLAGS) -c sftc.c

sftd.o: sfts.c
	$(CC) $(CFLAGS) -c sfts.c

err.o: err.c
	$(CC) $(CFLAGS) -c err.c

pdu.o: pdu.c
	$(CC) $(CFLAGS) -c pdu.c

cmd.o: cmd.c
	$(CC) $(CFLAGS) -c cmd.c

clean:
	rm *.o sftd sftc
