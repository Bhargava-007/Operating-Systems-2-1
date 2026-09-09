#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SERVER_FIFO "/tmp/server_fifo"
#define MAX_MSG 256

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <message>\n", argv[0]);
        exit(1);
    }

    int fd = open(SERVER_FIFO, O_WRONLY);
    if (fd == -1) {
        perror("Cannot open FIFO - is server running?");
        exit(1);
    }

    char buffer[MAX_MSG];
    snprintf(buffer, sizeof(buffer), "Client PID %d says: %s\n", getpid(), argv[1]);
    write(fd, buffer, strlen(buffer));
    printf("[Client PID %d] Sent: %s", getpid(), buffer);
    fflush(stdout);

    close(fd);
    return 0;
}
