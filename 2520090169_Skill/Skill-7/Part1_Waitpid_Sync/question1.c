#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("========================================\n");
    printf("  Process Sync using waitpid()\n");
    printf("========================================\n\n");

    pid_t pids[3];

    for (int i = 0; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] < 0) { perror("fork"); exit(1); }
        if (pids[i] == 0) {
            printf("[Child %d PID:%d] Started, sleeping %ds\n", i+1, getpid(), i+1);
            sleep(i + 1);
            printf("[Child %d PID:%d] Done\n", i+1, getpid());
            exit(i + 10);
        }
    }

    /* Wait for each child specifically using waitpid */
    for (int i = 0; i < 3; i++) {
        int status;
        pid_t done = waitpid(pids[i], &status, 0);
        if (WIFEXITED(status))
            printf("[Parent] Child PID:%d exited with code %d\n", done, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("[Parent] Child PID:%d killed by signal %d\n", done, WTERMSIG(status));
    }

    printf("\n[Parent PID:%d] All children synced.\n", getpid());
    return 0;
}
