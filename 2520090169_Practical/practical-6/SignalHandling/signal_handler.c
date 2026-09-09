#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_signal(int signum) {
    switch (signum) {
        case SIGINT:
            printf("\n[Handler] Caught SIGINT (Ctrl+C) - Signal %d\n", signum);
            printf("[Handler] Program continues running...\n\n");
            fflush(stdout);
            break;
        case SIGTERM:
            printf("\n[Handler] Caught SIGTERM - Signal %d\n", signum);
            printf("[Handler] Cleaning up and exiting.\n");
            fflush(stdout);
            exit(0);
        case SIGUSR1:
            printf("\n[Handler] Caught SIGUSR1 - Signal %d\n", signum);
            printf("[Handler] Custom user action triggered!\n\n");
            fflush(stdout);
            break;
    }
}

int main() {
    signal(SIGINT,  handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGUSR1, handle_signal);

    printf("[Main] PID: %d\n", getpid());
    printf("[Main] Handlers registered for SIGINT, SIGTERM, SIGUSR1\n");
    printf("[Main] Press Ctrl+C to send SIGINT\n");
    printf("[Main] From another terminal run:\n");
    printf("       kill -SIGUSR1 %d\n", getpid());
    printf("       kill -SIGTERM %d\n\n", getpid());

    while (1) {
        printf("[Main] Running... (sleeping 3s)\n");
        fflush(stdout);
        sleep(3);
    }

    return 0;
}
