#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE  512
#define MAX_BLOCKS   20

typedef struct {
    int  pid;
    int  start;
    int  size;
    int  free;
} MemBlock;

MemBlock memory[MAX_BLOCKS];
int      block_count = 0;

void init_memory() {
    memory[0].pid   = -1;
    memory[0].start =  0;
    memory[0].size  = MEMORY_SIZE;
    memory[0].free  = 1;
    block_count     = 1;
    printf("[Memory] Initialized: %d KB total\n\n", MEMORY_SIZE);
}

void display_memory() {
    printf("\n--- Memory Map ---\n");
    printf("%-8s %-8s %-8s %-8s\n", "Start","Size","PID","Status");
    for (int i = 0; i < block_count; i++) {
        printf("%-8d %-8d %-8d %-8s\n",
               memory[i].start,
               memory[i].size,
               memory[i].free ? -1 : memory[i].pid,
               memory[i].free ? "FREE" : "USED");
    }
    printf("------------------\n");
}

int allocate(int pid, int size) {
    for (int i = 0; i < block_count; i++) {
        if (memory[i].free && memory[i].size >= size) {
            // split block if leftover
            if (memory[i].size > size && block_count < MAX_BLOCKS) {
                for (int j = block_count; j > i+1; j--)
                    memory[j] = memory[j-1];
                memory[i+1].start = memory[i].start + size;
                memory[i+1].size  = memory[i].size  - size;
                memory[i+1].free  = 1;
                memory[i+1].pid   = -1;
                block_count++;
            }
            memory[i].free = 0;
            memory[i].pid  = pid;
            memory[i].size = size;
            printf("[Memory] Allocated %d KB to PID %d at offset %d\n",
                   size, pid, memory[i].start);
            return 1;
        }
    }
    printf("[Memory] ERROR: Not enough memory for PID %d (%d KB)\n", pid, size);
    return 0;
}

void deallocate(int pid) {
    for (int i = 0; i < block_count; i++) {
        if (!memory[i].free && memory[i].pid == pid) {
            memory[i].free = 1;
            memory[i].pid  = -1;
            printf("[Memory] Freed block at offset %d (PID %d)\n",
                   memory[i].start, pid);

            // merge with next if free
            if (i+1 < block_count && memory[i+1].free) {
                memory[i].size += memory[i+1].size;
                for (int j = i+1; j < block_count-1; j++)
                    memory[j] = memory[j+1];
                block_count--;
            }
            // merge with prev if free
            if (i > 0 && memory[i-1].free) {
                memory[i-1].size += memory[i].size;
                for (int j = i; j < block_count-1; j++)
                    memory[j] = memory[j+1];
                block_count--;
            }
            return;
        }
    }
    printf("[Memory] PID %d not found\n", pid);
}

int main() {
    printf("=== NanoKernel Memory Manager ===\n");
    init_memory();
    display_memory();

    allocate(1, 100);
    allocate(2, 200);
    allocate(3,  50);
    display_memory();

    deallocate(2);
    display_memory();

    allocate(4, 180);
    display_memory();

    deallocate(1);
    deallocate(3);
    deallocate(4);
    display_memory();

    return 0;
}
