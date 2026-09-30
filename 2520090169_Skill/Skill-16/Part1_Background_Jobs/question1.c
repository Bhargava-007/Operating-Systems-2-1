#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 4

typedef struct { pid_t pid; char cmd[64]; } Job;
Job jobs[MAX];
int jcount = 0;

void launch_bg(char *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args);
        exit(1);
    }
    jobs[jcount].pid = pid;
    snprintf(jobs[jcount].cmd, 64, "%s", cmd);
    jcount++;
    printf("[%d] %d launched: %s\n", jcount, pid, cmd);
}

void monitor() {
    printf("\n--- Monitoring jobs ---\n");
    for (int i = 0; i < jcount; i++) {
        int status;
        pid_t r = waitpid(jobs[i].pid, &status, WNOHANG);
        if (r == 0) printf("[%d] %d running: %s\n", i+1, jobs[i].pid, jobs[i].cmd);
        else        printf("[%d] %d done:    %s\n", i+1, jobs[i].pid, jobs[i].cmd);
    }
}

int main() {
    printf("=== Background Jobs ===\n\n");
    launch_bg("sleep 2 && echo job1 done");
    launch_bg("echo job2 immediate");
    launch_bg("sleep 1 && echo job3 done");
    usleep(500000);
    monitor();
    printf("\nPrompt returned immediately after launch.\n");
    sleep(3);
    monitor();
    return 0;
}
