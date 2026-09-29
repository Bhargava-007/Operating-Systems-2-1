CC = gcc
CFLAGS = -Wall -pthread
TARGET = pc
OBJS = main.o buffer.o producer.o consumer.o semaphores.o logger.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: all
	./$(TARGET) 5 2 2 4
