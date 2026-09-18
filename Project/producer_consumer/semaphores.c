#include "semaphores.h"

sem_t empty, full, mutex;

void init_semaphores(int buffer_size) {
    sem_init(&empty, 0, buffer_size);
    sem_init(&full,  0, 0);
    sem_init(&mutex, 0, 1);
}

void destroy_semaphores() {
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
}
