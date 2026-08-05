#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 == 0)
    {
        printf("Child 1: PID = %d\n", getpid());
        sleep(30);
        printf("Child 1 exiting.\n");
        exit(0);
    }

    pid2 = fork();

    if (pid2 == 0)
    {
        printf("Child 2: PID = %d\n", getpid());
        sleep(60);
        printf("Child 2 exiting.\n");
        exit(0);
    }

    printf("Parent PID: %d\n", getpid());

    printf("Waiting specifically for Child 1 using waitpid()...\n");
    waitpid(pid1, NULL, 0);
    printf("Child 1 finished.\n");

    printf("Waiting for remaining child using wait()...\n");
    wait(NULL);
    printf("Child 2 finished.\n");

    printf("Parent exiting.\n");

    return 0;
}