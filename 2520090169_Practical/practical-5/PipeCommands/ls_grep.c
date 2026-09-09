#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    printf("[Main] Executing: ls -l | grep .c\n\n");
    fflush(stdout);

    pid_t pid1 = fork();
    if (pid1 == -1) { perror("fork1 failed"); exit(1); }

    if (pid1 == 0) {
        // CHILD 1 — runs ls -l
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to pipe write
        close(pipefd[1]);
        execlp("ls", "ls", "-l", NULL);
        perror("exec ls failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) { perror("fork2 failed"); exit(1); }

    if (pid2 == 0) {
        // CHILD 2 — runs grep .c
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO); // redirect stdin to pipe read
        close(pipefd[0]);
        execlp("grep", "grep", ".c", NULL);
        perror("exec grep failed");
        exit(1);
    }

    // Parent closes both ends and waits
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);

    printf("\n[Main] Done.\n");
    return 0;
}
