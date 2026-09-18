#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int shared_data = 100;

void print_memory_usage(const char *label, pid_t pid) {
    char path[64];
    char line[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *f = fopen(path, "r");
    if (!f) { perror("fopen"); return; }
    printf("\n--- %s (PID: %d) ---\n", label, pid);
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "VmRSS:", 6) == 0 ||
            strncmp(line, "VmSize:", 7) == 0 ||
            strncmp(line, "RssAnon:", 8) == 0 ||
            strncmp(line, "RssFile:", 8) == 0)
            printf("  %s", line);
    }
    fclose(f);
}

int main() {
    printf("========================================\n");
    printf("   Copy-on-Write (CoW) Demo\n");
    printf("========================================\n");

    int *heap_data = (int *)malloc(4096 * sizeof(int));
    for (int i = 0; i < 4096; i++) heap_data[i] = i;

    printf("\n[Parent] PID: %d\n", getpid());
    printf("[Parent] shared_data address : %p  value: %d\n", (void *)&shared_data, shared_data);
    printf("[Parent] heap_data address   : %p  value[0]: %d\n", (void *)heap_data, heap_data[0]);
    print_memory_usage("Parent BEFORE fork", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* CHILD PROCESS */
        printf("\n========================================\n");
        printf("[Child] PID: %d  Parent PID: %d\n", getpid(), getppid());
        printf("[Child] shared_data address : %p  value: %d\n", (void *)&shared_data, shared_data);
        printf("[Child] heap_data address   : %p  value[0]: %d\n", (void *)heap_data, heap_data[0]);
        print_memory_usage("Child BEFORE modification", getpid());

        /* Modify data - triggers Copy-on-Write */
        printf("\n[Child] Modifying shared_data and heap_data...\n");
        shared_data = 999;
        heap_data[0] = 9999;

        printf("[Child] After modification:\n");
        printf("[Child] shared_data address : %p  value: %d\n", (void *)&shared_data, shared_data);
        printf("[Child] heap_data address   : %p  value[0]: %d\n", (void *)heap_data, heap_data[0]);
        print_memory_usage("Child AFTER modification", getpid());

        free(heap_data);
        exit(0);
    } else {
        /* PARENT PROCESS */
        wait(NULL);
        printf("\n========================================\n");
        printf("[Parent] After child finished:\n");
        printf("[Parent] shared_data address : %p  value: %d (unchanged)\n", (void *)&shared_data, shared_data);
        printf("[Parent] heap_data address   : %p  value[0]: %d (unchanged)\n", (void *)heap_data, heap_data[0]);
        print_memory_usage("Parent AFTER child exit", getpid());

        printf("\n========================================\n");
        printf("  CoW Explanation\n");
        printf("========================================\n");
        printf("- Before fork: parent & child share same physical pages\n");
        printf("- Child modified data: OS created a NEW copy for child\n");
        printf("- Parent data remains UNCHANGED (CoW behavior)\n");
        printf("- Same virtual address, different physical page after write\n");

        free(heap_data);
    }

    return 0;
}
