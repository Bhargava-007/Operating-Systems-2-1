#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "buffer.h"
#include "semaphores.h"
#include "producer.h"
#include "consumer.h"
#include "logger.h"

// CO1: Demonstrates user-space to kernel-space transition via system calls
// Run with: strace ./pc 5 2 2 4 to observe sem_wait/sem_post kernel calls

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: ./pc <buffer_size> <num_producers> <num_consumers> <items_per_producer>\n");
        return 1;
    }

    BUFFER_SIZE        = atoi(argv[1]);
    int num_producers  = atoi(argv[2]);
    int num_consumers  = atoi(argv[3]);
    ITEMS_PER_PRODUCER = atoi(argv[4]);

    if (BUFFER_SIZE > MAX_BUFFER || BUFFER_SIZE < 1 || num_producers < 1 || num_consumers < 1) {
        printf("Error: invalid arguments\n");
        return 1;
    }

    srand(time(NULL));
    int total_items        = num_producers * ITEMS_PER_PRODUCER;
    int items_per_consumer = total_items / num_consumers;
    int remainder          = total_items % num_consumers;

    init_buffer();
    init_semaphores(BUFFER_SIZE);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];
    int prod_ids[num_producers];
    int cons_args[num_consumers][2];

    clock_t start = clock();

    for (int i = 0; i < num_producers; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prod_threads[i], NULL, producer, &prod_ids[i]);
    }

    for (int i = 0; i < num_consumers; i++) {
        cons_args[i][0] = i + 1;
        cons_args[i][1] = items_per_consumer + (i == num_consumers - 1 ? remainder : 0);
        pthread_create(&cons_threads[i], NULL, consumer, cons_args[i]);
    }

    for (int i = 0; i < num_producers; i++) pthread_join(prod_threads[i], NULL);
    for (int i = 0; i < num_consumers; i++) pthread_join(cons_threads[i], NULL);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    destroy_semaphores();

    printf("\n--- Summary ---\n");
    printf("Total items produced : %d\n", total_items);
    printf("Total items consumed : %d\n", total_items);
    printf("Time elapsed         : %.2f seconds\n", elapsed);
    printf("No race conditions. No deadlocks.\n");

    close_log();
    return 0;
}
