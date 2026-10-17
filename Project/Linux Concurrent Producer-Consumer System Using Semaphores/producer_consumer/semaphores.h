#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <semaphore.h>

extern sem_t empty;
extern sem_t full;
extern sem_t mutex;

void init_semaphores(int buffer_size);
void destroy_semaphores(void);

// Safe POSIX wait handling interrupted system calls (EINTR)
int safe_sem_wait(sem_t *sem);

#endif /* SEMAPHORES_H */
