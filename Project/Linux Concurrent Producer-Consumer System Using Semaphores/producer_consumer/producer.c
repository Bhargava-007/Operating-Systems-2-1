#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "producer.h"
#include "buffer.h"
#include "semaphores.h"
#include "logger.h"

void *producer(void *arg) {
    // CO2: Thread lifecycle - transitions from Ready to Running to Blocked (sem_wait) to Ready
    producer_arg_t *parg = (producer_arg_t *)arg;
    int id = parg->id;
    int items = parg->items_to_produce;
    const char *session_id = parg->session_id;

    for (int i = 0; i < items; i++) {
        int item = (rand() % 1000) + 1; // 1-1000 payload

        // 1. Wait for available slot in buffer (blocking wait on counting semaphore)
        safe_sem_wait(&empty);

        // 2. Acquire mutual exclusion binary semaphore
        safe_sem_wait(&mutex);

        // Critical section
        int slot = in;
        int res = insert_item(item);
        if (res != 0) {
            fprintf(stderr, "[ERROR] Producer %d detected buffer overflow/invariant violation at slot %d\n", id, slot);
        }

        printf("[Producer %d] Produced: %d at slot %d (occupancy: %d/%d)\n", id, item, slot, count, BUFFER_SIZE);
        fflush(stdout);
        log_event(session_id, "Producer", id, "INSERT", item, slot, count);

        // Exit critical section
        sem_post(&mutex);

        // Signal consumer that a new full slot is ready
        sem_post(&full);

        // Small sleep to permit thread interleaving (10ms)
        usleep(10000);
    }
    return NULL;
}
