CC     = gcc
CFLAGS = -Wall -g

all: scheduler memory

scheduler: scheduler.c
	$(CC) $(CFLAGS) -o scheduler scheduler.c

memory: memory.c
	$(CC) $(CFLAGS) -o memory memory.c

clean:
	rm -f scheduler memory
