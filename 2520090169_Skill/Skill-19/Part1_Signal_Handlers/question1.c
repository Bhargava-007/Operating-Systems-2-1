#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile int running = 1;

void handle_sigint(int sig) {
    printf("\n[Handler] SIGINT(%d) caught - ignoring\n", sig);
}

void handle_sigusr1(int sig) {
    printf("[Handler] SIGUSR1(%d) caught\n", sig);
}

void handle_sigterm(int sig) {
    printf("[Handler] SIGTERM(%d) caught - shutting down\n", sig);
    running = 0;
}

int main() {
    printf("=== Signal Handlers ===\n");
    printf("PID: %d\n\n", getpid());

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &sa, NULL);

    printf("Sending SIGINT...\n");
    kill(getpid(), SIGINT);

    printf("Sending SIGUSR1...\n");
    kill(getpid(), SIGUSR1);

    printf("Sending SIGTERM...\n");
    kill(getpid(), SIGTERM);

    printf("\nAll signals handled. Stable.\n");
    return 0;
}
