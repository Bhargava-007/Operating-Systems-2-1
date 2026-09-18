#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Global variable - BSS segment (uninitialized) */
int global_uninit;

/* Global variable - Data segment (initialized) */
int global_init = 42;

int main() {
    /* Static variable - Data segment */
    static int static_var = 100;

    /* Stack variable */
    int stack_var = 200;

    /* Heap variable */
    int *heap_var = (int *)malloc(sizeof(int));
    *heap_var = 300;

    printf("========================================\n");
    printf("  Linux Process Address Space Layout\n");
    printf("========================================\n");
    printf("PID: %d\n\n", getpid());

    printf("--- Memory Segment Addresses ---\n");
    printf("[Code/Text]   main() function   : %p\n", (void *)main);
    printf("[Data]        global_init        : %p  (value=%d)\n", (void *)&global_init, global_init);
    printf("[BSS]         global_uninit      : %p  (value=%d)\n", (void *)&global_uninit, global_uninit);
    printf("[Data]        static_var         : %p  (value=%d)\n", (void *)&static_var, static_var);
    printf("[Heap]        heap_var           : %p  (value=%d)\n", (void *)heap_var, *heap_var);
    printf("[Stack]       stack_var          : %p  (value=%d)\n", (void *)&stack_var, stack_var);

    printf("\n--- Address Order ---\n");
    printf("Text(lowest) -> Data -> BSS -> Heap -> ... -> Stack(highest)\n");

    free(heap_var);
    return 0;
}
