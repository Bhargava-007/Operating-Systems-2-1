#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Before execl()\n");
    execl("/bin/ls", "ls", "-l", NULL);
    printf("This will not be printed\n");
    return 0;
}