#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void run_in_group(char *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        setpgid(0, 0);
        printf("[Child] PID=%d PGID=%d cmd=%s\n", getpid(), getpgrp(), cmd);
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args);
        exit(1);
    }
    setpgid(pid, pid);
    printf("[Shell] Launched PID=%d PGID=%d\n", pid, pid);
    waitpid(pid, NULL, 0);
}

void signal_group(pid_t pgid, int sig) {
    printf("[Shell] Sending signal %d to group %d\n", sig, pgid);
    killpg(pgid, sig);
}

int main() {
    printf("=== Process Groups ===\n");
    printf("Shell PID=%d PGID=%d\n\n", getpid(), getpgrp());

    printf("Test 1: run in own group\n");
    run_in_group("echo hello_from_group");

    printf("\nTest 2: create group and signal it\n");
    pid_t pid = fork();
    if (pid == 0) {
        setpgid(0, 0);
        printf("[Child] PID=%d PGID=%d sleeping...\n", getpid(), getpgrp());
        sleep(5);
        exit(0);
    }
    setpgid(pid, pid);
    sleep(1);
    signal_group(pid, SIGTERM);
    waitpid(pid, NULL, 0);
    printf("[Shell] Group %d terminated\n", pid);

    printf("\nTerminal control restored to shell PGID=%d\n", getpgrp());
    return 0;
}
