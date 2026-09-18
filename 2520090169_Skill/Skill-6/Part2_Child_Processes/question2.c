#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void launch(char *cmd, char *args[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    }
    if (pid == 0) {
        printf("[Child PID:%d] Executing: %s\n", getpid(), cmd);
        execvp(cmd, args);
        perror("execvp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            printf("[Parent] Child PID:%d exited with status %d\n", pid, WEXITSTATUS(status));
    }
}

int main() {
    printf("========================================\n");
    printf("   Child Process & Execution Demo\n");
    printf("========================================\n\n");

    /* Test 1: ls */
    char *cmd1[] = {"ls", "-l", NULL};
    printf("--- Test 1: ls -l ---\n");
    launch("ls", cmd1);

    /* Test 2: echo */
    char *cmd2[] = {"echo", "Hello from child process!", NULL};
    printf("\n--- Test 2: echo ---\n");
    launch("echo", cmd2);

    /* Test 3: pwd */
    char *cmd3[] = {"pwd", NULL};
    printf("\n--- Test 3: pwd ---\n");
    launch("pwd", cmd3);

    /* Test 4: invalid command - error handling */
    char *cmd4[] = {"invalidcmd", NULL};
    printf("\n--- Test 4: invalid command ---\n");
    launch("invalidcmd", cmd4);

    printf("\n========================================\n");
    printf("[Parent PID:%d] All children done.\n", getpid());

    return 0;
}
