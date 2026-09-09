CC     = gcc
CFLAGS = -Wall -g

all: shellforge

shellforge: main.c shell.c shell.h
	$(CC) $(CFLAGS) -o shellforge main.c shell.c

clean:
	rm -f shellforge
