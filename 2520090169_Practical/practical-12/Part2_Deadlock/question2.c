#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t resourceA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resourceB = PTHREAD_MUTEX_INITIALIZER;

// --- DEADLOCK VERSION ---
void *thread1_deadlock(void *arg) {
    pthread_mutex_lock(&resourceA);
    printf("[Deadlock] Thread1 acquired A, waiting for B...\n");
    usleep(100000);
    pthread_mutex_lock(&resourceB);
    printf("[Deadlock] Thread1 acquired B\n");
    pthread_mutex_unlock(&resourceB);
    pthread_mutex_unlock(&resourceA);
    return NULL;
}

void *thread2_deadlock(void *arg) {
    pthread_mutex_lock(&resourceB);
    printf("[Deadlock] Thread2 acquired B, waiting for A...\n");
    usleep(100000);
    pthread_mutex_lock(&resourceA);
    printf("[Deadlock] Thread2 acquired A\n");
    pthread_mutex_unlock(&resourceA);
    pthread_mutex_unlock(&resourceB);
    return NULL;
}

// --- PREVENTION VERSION (resource ordering) ---
void *thread1_safe(void *arg) {
    pthread_mutex_lock(&resourceA);
    pthread_mutex_lock(&resourceB);
    printf("[Safe] Thread1 acquired A then B\n");
    pthread_mutex_unlock(&resourceB);
    pthread_mutex_unlock(&resourceA);
    return NULL;
}

void *thread2_safe(void *arg) {
    pthread_mutex_lock(&resourceA);
    pthread_mutex_lock(&resourceB);
    printf("[Safe] Thread2 acquired A then B\n");
    pthread_mutex_unlock(&resourceB);
    pthread_mutex_unlock(&resourceA);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    printf("=== Deadlock Scenario ===\n");
    printf("Four necessary conditions:\n");
    printf("1. Mutual Exclusion  - only one thread holds a resource\n");
    printf("2. Hold and Wait     - thread holds A while waiting for B\n");
    printf("3. No Preemption     - resources cannot be forcibly taken\n");
    printf("4. Circular Wait     - T1 waits for T2, T2 waits for T1\n\n");

    printf("--- Deadlock Demo (will hang ~3s then timeout) ---\n");
    pthread_create(&t1, NULL, thread1_deadlock, NULL);
    pthread_create(&t2, NULL, thread2_deadlock, NULL);
    sleep(3);
    printf("[Deadlock] Detected: threads are stuck. Skipping join.\n\n");

    pthread_mutex_unlock(&resourceA);
    pthread_mutex_unlock(&resourceB);
    pthread_mutex_unlock(&resourceA);
    pthread_mutex_unlock(&resourceB);

    printf("--- Prevention: Resource Ordering (always lock A before B) ---\n");
    pthread_create(&t1, NULL, thread1_safe, NULL);
    pthread_create(&t2, NULL, thread2_safe, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nPrevention successful: no deadlock with ordered locking.\n");

    pthread_mutex_destroy(&resourceA);
    pthread_mutex_destroy(&resourceB);
    return 0;
}
