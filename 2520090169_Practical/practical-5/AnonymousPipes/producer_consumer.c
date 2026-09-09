#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_ITEMS 5
#define MAX_MSG 256

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[MAX_MSG];

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    printf("[Main] Pipe created. Forking...\n");
    fflush(stdout);

    pid = fork();
    if (pid == -1) { perror("fork failed"); exit(1); }

    if (pid > 0) {
        // PARENT = Producer
        close(pipefd[0]); // close read end

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 1; i <= NUM_ITEMS; i++) {
            snprintf(buffer, sizeof(buffer), "Item-%d from Producer PID %d", i, getpid());
            write(pipefd[1], buffer, strlen(buffer) + 1);
            printf("[Producer] Sent: %s\n", buffer);
            fflush(stdout);
            sleep(1);
        }

        close(pipefd[1]);
        wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("\n[Main] Total communication time: %.3f seconds\n", elapsed);
        printf("[Main] Items transferred: %d\n", NUM_ITEMS);
        printf("[Main] Efficiency: %.3f seconds per item\n", elapsed / NUM_ITEMS);

    } else {
        // CHILD = Consumer
        close(pipefd[1]); // close write end

        int count = 0;
        while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
            printf("[Consumer] Received: %s\n", buffer);
            fflush(stdout);
            count++;
        }

        close(pipefd[0]);
        printf("[Consumer] Done. Consumed %d items.\n", count);
        exit(0);
    }

    return 0;
}
