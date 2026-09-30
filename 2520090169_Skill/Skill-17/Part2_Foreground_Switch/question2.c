#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 8

typedef struct {
    int id;
    pid_t pid;
    char cmd[64];
    char state[16];
} Job;

Job table[MAX];
int jcount = 0;

void add_job(char *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args);
        exit(1);
    }
    table[jcount].id  = jcount + 1;
    table[jcount].pid = pid;
    snprintf(table[jcount].cmd,   64, "%s", cmd);
    snprintf(table[jcount].state, 16, "running");
    jcount++;
    printf("[%d] %d running: %s\n", jcount, pid, cmd);
}

void list_jobs() {
    printf("\n--- Jobs ---\n");
    printf("%-4s %-8s %-10s %s\n", "ID", "PID", "STATE", "CMD");
    for (int i = 0; i < jcount; i++)
        printf("%-4d %-8d %-10s %s\n", table[i].id, table[i].pid, table[i].state, table[i].cmd);
}

void bring_fg(int id) {
    int idx = id - 1;
    if (idx < 0 || idx >= jcount) { printf("Invalid job id\n"); return; }
    printf("\nBringing [%d] to foreground: %s\n", id, table[idx].cmd);
    waitpid(table[idx].pid, NULL, 0);
    snprintf(table[idx].state, 16, "done");
    printf("[%d] done: %s\n", id, table[idx].cmd);
}

int main() {
    printf("=== Foreground Switch ===\n\n");
    add_job("sleep 2 && echo job1 done");
    add_job("echo job2 immediate");
    add_job("sleep 1 && echo job3 done");
    list_jobs();
    printf("\nfg 3\n");
    bring_fg(3);
    printf("\nfg 1\n");
    bring_fg(1);
    list_jobs();
    return 0;
}
