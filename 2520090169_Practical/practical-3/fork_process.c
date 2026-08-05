#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Fork failed\n");
        return 1;
    }

    if (pid == 0)
    {
        printf("\n----- CHILD PROCESS -----\n");
        printf("Child PID : %d\n", getpid());
        printf("Parent PID: %d\n", getppid());

        printf("Child is sleeping for 50 seconds...\n");
        sleep(50);

        printf("Child process exiting.\n");
    }
    else
    {
        printf("\n----- PARENT PROCESS -----\n");
        printf("Parent PID: %d\n", getpid());
        printf("Parent PPID: %d\n", getppid());
        printf("Child PID : %d\n", pid);

        printf("Parent waiting for child...\n");
        wait(NULL);

        printf("Child terminated.\n");
        printf("Parent exiting.\n");
    }

    return 0;
}