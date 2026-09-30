#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pid_t p1, p2;

    printf("=== Single Pipe Demo ===\n");
    printf("Command: ls | grep .c\n\n");

    pipe(fd);

    p1 = fork();
    if (p1 == 0) {
        dup2(fd[1], 1);
        close(fd[0]); close(fd[1]);
        execlp("ls", "ls", NULL);
    }

    p2 = fork();
    if (p2 == 0) {
        dup2(fd[0], 0);
        close(fd[1]); close(fd[0]);
        execlp("grep", "grep", ".c", NULL);
    }

    close(fd[0]); close(fd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);

    printf("\nPipe complete. Descriptors closed.\n");
    return 0;
}
