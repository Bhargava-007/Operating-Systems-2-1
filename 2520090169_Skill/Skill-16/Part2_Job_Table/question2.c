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
    printf("[%d] %d started: %s\n", jcount, pid, cmd);
}

void update_table() {
    for (int i = 0; i < jcount; i++) {
        if (!strcmp(table[i].state, "done")) continue;
        int status;
        pid_t r = waitpid(table[i].pid, &status, WNOHANG);
        if (r > 0) snprintf(table[i].state, 16, "done");
    }
}

void remove_done() {
    printf("\n--- Removing completed jobs ---\n");
    for (int i = 0; i < jcount; i++)
        if (!strcmp(table[i].state, "done"))
            printf("Removed [%d] %d: %s\n", table[i].id, table[i].pid, table[i].cmd);
}

void print_table() {
    printf("\n--- Job Table ---\n");
    printf("%-4s %-8s %-10s %s\n", "ID", "PID", "STATE", "CMD");
    for (int i = 0; i < jcount; i++)
        printf("%-4d %-8d %-10s %s\n", table[i].id, table[i].pid, table[i].state, table[i].cmd);
}

int main() {
    printf("=== Job Table ===\n\n");
    add_job("sleep 2");
    add_job("echo hello");
    add_job("sleep 1");
    print_table();
    usleep(500000);
    update_table();
    print_table();
    sleep(2);
    update_table();
    print_table();
    remove_done();
    return 0;
}
