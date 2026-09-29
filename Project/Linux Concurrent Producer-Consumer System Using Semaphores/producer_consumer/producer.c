#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "producer.h"
#include "buffer.h"
#include "semaphores.h"
#include "logger.h"

int ITEMS_PER_PRODUCER;

void *producer(void *arg) {
    // CO2: Thread lifecycle - transitions from Running to Blocked (sem_wait) to Ready
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = rand() % 100;
        sem_wait(&empty);
        sem_wait(&mutex);
        insert_item(item);
        printf("[Producer %d] Produced: %d at slot %d\n", id, item, (in - 1 + BUFFER_SIZE) % BUFFER_SIZE);
        log_event("Producer inserted item");
        sem_post(&mutex);
        sem_post(&full);
        usleep(100000);
    }
    return NULL;
}

