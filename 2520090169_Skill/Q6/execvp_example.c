#include <stdio.h>
#include <unistd.h>

int main() {

    char *args[] = {"ls", "-l", NULL};

    printf("Before execvp()\n");

    execvp("ls", args);

    return 0;
}