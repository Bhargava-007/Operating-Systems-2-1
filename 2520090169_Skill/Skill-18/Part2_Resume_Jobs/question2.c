#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 4

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

void stop_job(int id) {
    int i = id - 1;
    kill(table[i].pid, SIGSTOP);
    snprintf(table[i].state, 16, "stopped");
    printf("[%d] %d stopped\n", id, table[i].pid);
}

void resume_job(int id) {
    int i = id - 1;
    kill(table[i].pid, SIGCONT);
    snprintf(table[i].state, 16, "running");
    printf("[%d] %d resumed\n", id, table[i].pid);
}

void list_jobs() {
    printf("\n--- Jobs ---\n");
    for (int i = 0; i < jcount; i++)
        printf("[%d] %-8d %-10s %s\n", table[i].id, table[i].pid, table[i].state, table[i].cmd);
}

int main() {
    printf("=== Resume Stopped Jobs ===\n\n");
    add_job("sleep 5");
    add_job("sleep 5");
    list_jobs();
    sleep(1);
    stop_job(1);
    stop_job(2);
    list_jobs();
    sleep(1);
    resume_job(1);
    resume_job(2);
    list_jobs();
    kill(table[0].pid, SIGTERM);
    kill(table[1].pid, SIGTERM);
    printf("\nJobs terminated.\n");
    return 0;
}
