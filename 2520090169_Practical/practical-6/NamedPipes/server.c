#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SERVER_FIFO "/tmp/server_fifo"
#define MAX_MSG 256

int main() {
    char buffer[MAX_MSG];
    int fd;

    unlink(SERVER_FIFO);
    if (mkfifo(SERVER_FIFO, 0666) == -1) {
        perror("mkfifo failed");
        exit(1);
    }

    printf("[Server] FIFO created. Waiting for clients...\n");
    fflush(stdout);

    while (1) {
        fd = open(SERVER_FIFO, O_RDONLY);
        if (fd == -1) { perror("open failed"); break; }

        int bytes = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("[Server] Received: %s", buffer);
            printf("[Server] Processed: message length = %d bytes\n\n", bytes);
            fflush(stdout);
        }
        close(fd);
    }

    unlink(SERVER_FIFO);
    return 0;
}
