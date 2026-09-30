#include <stdio.h>
#include <pthread.h>

#define THREADS 4
#define ITERATIONS 100000

long counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < ITERATIONS; i++)
        counter++;
    return NULL;
}

int main() {
    pthread_t tid[THREADS];

    printf("=== Race Condition Demo ===\n");
    printf("Threads: %d, Iterations each: %d\n", THREADS, ITERATIONS);
    printf("Expected counter: %d\n", THREADS * ITERATIONS);

    for (int i = 0; i < THREADS; i++)
        pthread_create(&tid[i], NULL, increment, NULL);

    for (int i = 0; i < THREADS; i++)
        pthread_join(tid[i], NULL);

    printf("Actual counter:   %ld\n", counter);
    printf("Lost updates:     %ld\n", (long)(THREADS * ITERATIONS) - counter);
    return 0;
}
