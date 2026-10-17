CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
TARGET = pc
SRCS = main.c buffer.c semaphores.c producer.c consumer.c logger.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) pc_log.txt

.PHONY: all clean
