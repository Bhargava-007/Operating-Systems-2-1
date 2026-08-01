#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        sleep(2);
        printf("Child exiting.\n");
    }
    else {
        waitpid(pid, NULL, 0);
        printf("Parent waited for child PID: %d\n", pid);
    }

    return 0;
}