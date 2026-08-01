#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process running...\n");
        sleep(2);
        printf("Child process finished.\n");
    }
    else {
        wait(NULL);
        printf("Parent process resumes after child.\n");
    }

    return 0;
}