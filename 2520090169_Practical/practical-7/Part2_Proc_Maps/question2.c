#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_init = 99;
int global_uninit;

void read_proc_maps(pid_t pid) {
    char path[64];
    char line[512];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE *f = fopen(path, "r");
    if (!f) { perror("fopen"); return; }
    printf("\n========================================\n");
    printf("  /proc/%d/maps - Virtual Memory Map\n", pid);
    printf("========================================\n");
    printf("%-30s %-5s %-10s %s\n", "Address Range", "Perm", "Offset", "Mapping");
    printf("---------------------------------------------------------------\n");
    while (fgets(line, sizeof(line), f))
        printf("%s", line);
    fclose(f);
}

int main() {
    static int static_var = 55;
    int stack_var = 10;
    int *heap_var = (int *)malloc(1024);
    *heap_var = 77;

    pid_t pid = getpid();

    printf("========================================\n");
    printf("   Process Memory Analysis Report\n");
    printf("========================================\n");
    printf("PID: %d\n\n", pid);

    printf("--- Segment Addresses ---\n");
    printf("[Text]   main()        : %p\n", (void *)main);
    printf("[Data]   global_init   : %p\n", (void *)&global_init);
    printf("[BSS]    global_uninit : %p\n", (void *)&global_uninit);
    printf("[Data]   static_var    : %p\n", (void *)&static_var);
    printf("[Heap]   heap_var      : %p\n", (void *)heap_var);
    printf("[Stack]  stack_var     : %p\n", (void *)&stack_var);

    read_proc_maps(pid);

    printf("\n========================================\n");
    printf("  Segment Explanation\n");
    printf("========================================\n");
    printf("[Text]  : Read-only executable code\n");
    printf("[Data]  : Initialized global/static variables\n");
    printf("[BSS]   : Uninitialized global/static variables\n");
    printf("[Heap]  : Dynamic memory (malloc), grows upward\n");
    printf("[Stack] : Local variables, grows downward\n");
    printf("[VDSO]  : Virtual dynamic shared object (kernel)\n");
    printf("[VVAR]  : Kernel variables mapped to user space\n");

    free(heap_var);
    return 0;
}
