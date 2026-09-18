#include <stdio.h>
#include <unistd.h>
#include "consumer.h"
#include "buffer.h"
#include "semaphores.h"

void *consumer(void *arg) {
    int id    = ((int *)arg)[0];
    int total = ((int *)arg)[1];
    for (int i = 0; i < total; i++) {
        int item;
        sem_wait(&full);
        sem_wait(&mutex);
        int slot = out;
        remove_item(&item);
        printf("[Consumer %d] Consumed: %d from slot %d\n", id, item, slot);
        sem_post(&mutex);
        sem_post(&empty);
        usleep(150000);
    }
    return NULL;
}
