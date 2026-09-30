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
}

void update_states() {
    for (int i = 0; i < jcount; i++) {
        if (!strcmp(table[i].state, "done")) continue;
        if (waitpid(table[i].pid, NULL, WNOHANG) > 0)
            snprintf(table[i].state, 16, "done");
    }
}

void list_jobs() {
    printf("\n--- Active Jobs ---\n");
    printf("%-4s %-8s %-10s %s\n", "ID", "PID", "STATE", "CMD");
    for (int i = 0; i < jcount; i++)
        printf("%-4d %-8d %-10s %s\n", table[i].id, table[i].pid, table[i].state, table[i].cmd);
}

int main() {
    printf("=== List Jobs ===\n\n");
    add_job("sleep 3");
    add_job("echo done_immediately");
    add_job("sleep 1");
    list_jobs();
    sleep(1);
    update_states();
    printf("\nAfter 1s:\n");
    list_jobs();
    sleep(3);
    update_states();
    printf("\nAfter 3s:\n");
    list_jobs();
    return 0;
}
