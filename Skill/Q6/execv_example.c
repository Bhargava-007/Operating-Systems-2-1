#include <stdio.h>
#include <unistd.h>

int main() {

    char *args[] = {"ls", "-a", NULL};

    printf("Before execv()\n");

    execv("/bin/ls", args);

    return 0;
}