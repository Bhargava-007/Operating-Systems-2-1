CC = gcc
CFLAGS = -Wall -pthread
TARGET = pc

all: $(TARGET)

$(TARGET): producer_consumer.c
	$(CC) $(CFLAGS) -o $(TARGET) producer_consumer.c

clean:
	rm -f $(TARGET)
