#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "buffer.h"
#include "semaphores.h"
#include "producer.h"
#include "consumer.h"
#include "logger.h"

// CO1: Demonstrates user-space to kernel-space transition via system calls
// Run with: strace ./pc 5 2 2 4 to observe sem_wait/sem_post kernel calls

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > 6) {
        printf("Usage: ./pc <buffer_size> <num_producers> <num_consumers> <items_per_producer> [session_id]\n");
        return 1;
    }

    BUFFER_SIZE        = atoi(argv[1]);
    int num_producers  = atoi(argv[2]);
    int num_consumers  = atoi(argv[3]);
    int items_per_prod = atoi(argv[4]);
    const char *session_id = (argc == 6) ? argv[5] : "native_cli";

    if (BUFFER_SIZE > MAX_BUFFER || BUFFER_SIZE < 1 || num_producers < 1 || num_consumers < 1 || items_per_prod < 1) {
        printf("Error: invalid arguments. Limits: buffer_size (1-%d), producers>=1, consumers>=1, items>=1\n", MAX_BUFFER);
        return 1;
    }

    srand((unsigned int)time(NULL));
    int total_items        = num_producers * items_per_prod;
    int items_per_consumer = total_items / num_consumers;
    int remainder          = total_items % num_consumers;

    init_buffer();
    init_semaphores(BUFFER_SIZE);
    init_log(session_id);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];
    producer_arg_t prod_args[num_producers];
    consumer_arg_t cons_args[num_consumers];

    // Measure execution timing with CLOCK_MONOTONIC for nanosecond/millisecond precision
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < num_producers; i++) {
        prod_args[i].id = i + 1;
        prod_args[i].items_to_produce = items_per_prod;
        prod_args[i].session_id = session_id;
        pthread_create(&prod_threads[i], NULL, producer, &prod_args[i]);
    }

    for (int i = 0; i < num_consumers; i++) {
        cons_args[i].id = i + 1;
        cons_args[i].items_to_consume = items_per_consumer + (i == num_consumers - 1 ? remainder : 0);
        cons_args[i].session_id = session_id;
        pthread_create(&cons_threads[i], NULL, consumer, &cons_args[i]);
    }

    for (int i = 0; i < num_producers; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                        (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;

    // Read semaphore values directly to verify final invariants
    int s_empty = -1, s_full = -1, s_mutex = -1;
    sem_getvalue(&empty, &s_empty);
    sem_getvalue(&full, &s_full);
    sem_getvalue(&mutex, &s_mutex);

    destroy_semaphores();

    // Verifiable metrics calculation
    int buffer_invariants_hold = (count == 0) && (s_empty == BUFFER_SIZE) && (s_full == 0) && (s_mutex == 1);

    printf("\n--- Execution Metrics & Invariant Report ---\n");
    printf("Session ID               : %s\n", session_id);
    printf("Buffer Capacity          : %d\n", BUFFER_SIZE);
    printf("Producer Threads         : %d (total produced: %d)\n", num_producers, total_items);
    printf("Consumer Threads         : %d (total consumed: %d)\n", num_consumers, total_items);
    printf("Execution Duration       : %.3f ms (%.3f s)\n", elapsed_ms, elapsed_ms / 1000.0);
    printf("Final Buffer Occupancy   : %d items\n", count);
    printf("Final Semaphore Invariants: empty=%d/%d, full=%d/0, mutex=%d/1\n",
           s_empty, BUFFER_SIZE, s_full, s_mutex);

    if (buffer_invariants_hold) {
        printf("Verification Status      : PASS (All synchronization invariants satisfied, 0 deadlocks observed)\n");
    } else {
        printf("Verification Status      : FAIL (Invariant anomaly detected: count=%d, empty=%d, full=%d)\n",
               count, s_empty, s_full);
    }

    close_log();
    return buffer_invariants_hold ? 0 : 2;
}
