CC=gcc
CFLAGS=-g -Wall -O3 -D_REENTRANT -I ./libjoe
LDLIBS=-lpthread
OBJECTS=err.o pdu.o util.o sfts.o sftc.o sft.o ./libjoe/*.o

all: libjoe server client tester

libjoe:
	make -C libjoe

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

util.o: util.c
	$(CC) $(CFLAGS) -c util.c

tester: tester.c $(OBJECTS)
	$(CC) $(CFLAGS) -c tester.c
	$(CC) $(LDLIBS) -o tester $(OBJECTS) tester.o

clean:
	rm *.o sft_server sft_client tester
