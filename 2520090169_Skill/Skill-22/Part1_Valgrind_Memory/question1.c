#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void leaky() {
    char *p = malloc(64);
    strcpy(p, "this memory is never freed");
    printf("Leaky alloc: %s\n", p);
    // intentional leak - no free
}

void fixed() {
    char *p = malloc(64);
    strcpy(p, "this memory is properly freed");
    printf("Fixed alloc: %s\n", p);
    free(p);
}

void invalid_read() {
    int *arr = malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) arr[i] = i;
    printf("Valid read: arr[4]=%d\n", arr[4]);
    free(arr);
}

int main() {
    printf("=== Memory Management Demo ===\n\n");

    printf("-- Leak Demo --\n");
    leaky();

    printf("\n-- Fixed Alloc --\n");
    fixed();

    printf("\n-- Valid Array Access --\n");
    invalid_read();

    printf("\nRun with valgrind to detect leaks:\n");
    printf("valgrind --leak-check=full ./question1\n");
    return 0;
}
