#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <semaphore.h>

extern sem_t empty, full, mutex;

void init_semaphores(int buffer_size);
void destroy_semaphores();

#endif
