#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("========================================\n");
    printf("   Memory Allocation Demo\n");
    printf("========================================\n");

    /* malloc - allocates uninitialized memory */
    printf("\n--- malloc() ---\n");
    int *arr_malloc = (int *)malloc(5 * sizeof(int));
    if (!arr_malloc) { perror("malloc"); return 1; }
    for (int i = 0; i < 5; i++) arr_malloc[i] = (i + 1) * 10;
    printf("malloc allocated 5 ints at : %p\n", (void *)arr_malloc);
    for (int i = 0; i < 5; i++) printf("  arr_malloc[%d] = %d\n", i, arr_malloc[i]);

    /* calloc - allocates zero-initialized memory */
    printf("\n--- calloc() ---\n");
    int *arr_calloc = (int *)calloc(5, sizeof(int));
    if (!arr_calloc) { perror("calloc"); return 1; }
    printf("calloc allocated 5 ints at : %p (all zeros)\n", (void *)arr_calloc);
    for (int i = 0; i < 5; i++) printf("  arr_calloc[%d] = %d\n", i, arr_calloc[i]);

    /* realloc - resize previously allocated memory */
    printf("\n--- realloc() ---\n");
    arr_malloc = (int *)realloc(arr_malloc, 10 * sizeof(int));
    if (!arr_malloc) { perror("realloc"); return 1; }
    for (int i = 5; i < 10; i++) arr_malloc[i] = (i + 1) * 10;
    printf("realloc resized to 10 ints at : %p\n", (void *)arr_malloc);
    for (int i = 0; i < 10; i++) printf("  arr_malloc[%d] = %d\n", i, arr_malloc[i]);

    /* free - release memory */
    printf("\n--- free() ---\n");
    free(arr_malloc);
    printf("arr_malloc freed successfully\n");
    free(arr_calloc);
    printf("arr_calloc freed successfully\n");

    /* Intentional memory leak for valgrind demo */
    printf("\n--- Intentional Leak (for Valgrind) ---\n");
    int *leaked = (int *)malloc(10 * sizeof(int));
    printf("Leaked memory at : %p (not freed!)\n", (void *)leaked);

    printf("\n========================================\n");
    printf("  Program Ended - Check Valgrind Output\n");
    printf("========================================\n");

    return 0;
}
