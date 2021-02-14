CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu11
LDFLAGS = -lm -lpthread
TARGETS = sysprak-client
SRCFILES = sysprak-client.c performConnection.c config.c sharedMemory.c printBoard.c thinker.c
OBJECTS = sysprak-client.o performConnection.o config.o sharedMemory.o printBoard.o thinker.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGETS) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o $(TARGETS)

play:
	./sysprak-client -g $(GAME_ID) -p $(PLAYER) -c $(CONF_FILE)