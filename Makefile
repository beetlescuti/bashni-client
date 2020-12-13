CC = gcc
CFLAGS = -Wall -g -std=gnu11
LDFLAGS = -lm -lpthread
TARGETS = sysprak-client
SRCFILES = sysprak-client.c performConnection.c
OBJECTS = sysprak-client.o performConnection.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGETS) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o $(TARGETS)

play:
	./sysprak-client -g $(GAME_ID) -p $(PLAYER)