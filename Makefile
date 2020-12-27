CC = gcc
CFLAGS = -Wall -g -std=gnu11
LDFLAGS = -lm -lpthread
TARGETS = sysprakclient
SRCFILES = sysprakclient.c performConnection.c config.c
OBJECTS = sysprakclient.o performConnection.o config.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGETS) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o $(TARGETS)

play:
	./sysprakclient -g $(GAME_ID) -p $(PLAYER) -c $(CONF_FILE)