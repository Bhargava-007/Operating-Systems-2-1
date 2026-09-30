#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void handler(int sig) {
    printf("Received signal: %d\n", sig);
}

int main() {
    printf("=== Signal Analysis ===\n\n");
    printf("PID: %d  PGID: %d\n", getpid(), getpgrp());

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    signal(SIGTERM, handler);

    sigset_t mask, old;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &old);
    printf("SIGUSR1 blocked. Sending SIGUSR1...\n");
    kill(getpid(), SIGUSR1);
    printf("SIGUSR1 pending (blocked)\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    printf("SIGUSR1 unblocked - delivered now\n");

    printf("\nSending SIGUSR2...\n");
    kill(getpid(), SIGUSR2);

    printf("\nTerminal signals: SIGINT=2 SIGTSTP=20 SIGHUP=1\n");
    printf("Process group %d controls terminal signals.\n", getpgrp());
    return 0;
}
