#include <stdio.h>
#include <unistd.h>
#include "consumer.h"
#include "buffer.h"
#include "semaphores.h"
#include "logger.h"

void *consumer(void *arg) {
    // CO2: Thread lifecycle - transitions from Ready to Running to Blocked (sem_wait) to Ready
    consumer_arg_t *carg = (consumer_arg_t *)arg;
    int id = carg->id;
    int total = carg->items_to_consume;
    const char *session_id = carg->session_id;

    for (int i = 0; i < total; i++) {
        int item = -1;

        // 1. Wait for available item in buffer (blocking wait on counting semaphore)
        safe_sem_wait(&full);

        // 2. Acquire mutual exclusion binary semaphore
        safe_sem_wait(&mutex);

        // Critical section
        int slot = out;
        int res = remove_item(&item);
        if (res != 0) {
            fprintf(stderr, "[ERROR] Consumer %d detected buffer underflow/empty extraction at slot %d\n", id, slot);
        }

        printf("[Consumer %d] Consumed: %d from slot %d (occupancy: %d/%d)\n", id, item, slot, count, BUFFER_SIZE);
        fflush(stdout);
        log_event(session_id, "Consumer", id, "REMOVE", item, slot, count);

        // Exit critical section
        sem_post(&mutex);

        // Signal producer that an empty slot is now available
        sem_post(&empty);

        // Small sleep to permit thread interleaving (12ms)
        usleep(12000);
    }
    return NULL;
}
