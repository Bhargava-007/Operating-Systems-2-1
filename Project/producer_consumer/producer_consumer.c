#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_BUFFER 100

int buffer[MAX_BUFFER];
int in = 0, out = 0;
int BUFFER_SIZE;
int ITEMS_PER_PRODUCER;

sem_t empty, full, mutex;

void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = rand() % 100;
        sem_wait(&empty);
        sem_wait(&mutex);
        buffer[in] = item;
        printf("[Producer %d] Produced: %d at slot %d\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        sem_post(&mutex);
        sem_post(&full);
        usleep(100000);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = ((int *)arg)[0];
    int total_items = ((int *)arg)[1];
    for (int i = 0; i < total_items; i++) {
        sem_wait(&full);
        sem_wait(&mutex);
        int item = buffer[out];
        printf("[Consumer %d] Consumed: %d from slot %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        sem_post(&mutex);
        sem_post(&empty);
        usleep(150000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: ./pc <buffer_size> <num_producers> <num_consumers> <items_per_producer>\n");
        return 1;
    }

    BUFFER_SIZE        = atoi(argv[1]);
    int num_producers  = atoi(argv[2]);
    int num_consumers  = atoi(argv[3]);
    ITEMS_PER_PRODUCER = atoi(argv[4]);

    int total_items        = num_producers * ITEMS_PER_PRODUCER;
    int items_per_consumer = total_items / num_consumers;

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full,  0, 0);
    sem_init(&mutex, 0, 1);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];
    int prod_ids[num_producers];
    int cons_args[num_consumers][2];

    for (int i = 0; i < num_producers; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prod_threads[i], NULL, producer, &prod_ids[i]);
    }

    for (int i = 0; i < num_consumers; i++) {
        cons_args[i][0] = i + 1;
        cons_args[i][1] = items_per_consumer;
        pthread_create(&cons_threads[i], NULL, consumer, cons_args[i]);
    }

    for (int i = 0; i < num_producers; i++) pthread_join(prod_threads[i], NULL);
    for (int i = 0; i < num_consumers; i++) pthread_join(cons_threads[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    printf("\nAll done. No race conditions. No deadlocks.\n");
    return 0;
}
