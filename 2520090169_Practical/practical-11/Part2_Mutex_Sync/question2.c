#include <stdio.h>
#include <pthread.h>

#define THREADS 4
#define ITERATIONS 100000

long counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t tid[THREADS];

    printf("=== Mutex Synchronized Counter ===\n");
    printf("Threads: %d, Iterations each: %d\n", THREADS, ITERATIONS);
    printf("Expected counter: %d\n", THREADS * ITERATIONS);

    for (int i = 0; i < THREADS; i++)
        pthread_create(&tid[i], NULL, increment, NULL);

    for (int i = 0; i < THREADS; i++)
        pthread_join(tid[i], NULL);

    printf("Actual counter:   %ld\n", counter);
    printf("Lost updates:     %ld\n", (long)(THREADS * ITERATIONS) - counter);

    pthread_mutex_destroy(&lock);

    printf("\n=== Analysis ===\n");
    printf("Without mutex: counter < expected due to race conditions.\n");
    printf("With mutex:    counter == expected, no lost updates.\n");
    return 0;
}
