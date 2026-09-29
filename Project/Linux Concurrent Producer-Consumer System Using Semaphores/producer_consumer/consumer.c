#include <stdio.h>
#include <unistd.h>
#include "consumer.h"
#include "buffer.h"
#include "semaphores.h"
#include "logger.h"

void *consumer(void *arg) {
    // CO2: Thread lifecycle - transitions from Running to Blocked (sem_wait) to Ready
    int id    = ((int *)arg)[0];
    int total = ((int *)arg)[1];
    for (int i = 0; i < total; i++) {
        int item;
        sem_wait(&full);
        sem_wait(&mutex);
        int slot = out;
        remove_item(&item);
        printf("[Consumer %d] Consumed: %d from slot %d\n", id, item, slot);
        log_event("Consumer removed item");
        sem_post(&mutex);
        sem_post(&empty);
        usleep(150000);
    }
    return NULL;
}

