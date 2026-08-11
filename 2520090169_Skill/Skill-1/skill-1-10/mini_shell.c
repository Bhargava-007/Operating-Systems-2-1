#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    char command[100];

    printf("Enter a Linux command: ");
    scanf("%99s", command);

    pid_t pid = fork();

    if (pid == 0) {
        printf("\nChild Process\n");
        printf("Child PID : %d\n", getpid());

        execlp(command, command, NULL);

        perror("Command execution failed");
        exit(1);
    }
    else if (pid > 0) {
        printf("\nParent Process\n");
        printf("Parent PID : %d\n", getpid());

        wait(NULL);

        printf("Child process completed.\n");
    }
    else {
        printf("Fork failed.\n");
    }

    return 0;
}