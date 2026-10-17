CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = scheduler
SRCS = scheduler.c dynamic_priority.c

all: $(TARGET)

$(TARGET): $(SRCS) process.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
