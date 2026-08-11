#include <stdio.h>
#include <unistd.h>

int main() {

    fork();

    fork();

    printf("PID: %d | Parent PID: %d\n", getpid(), getppid());

    sleep(20);

    return 0;
}