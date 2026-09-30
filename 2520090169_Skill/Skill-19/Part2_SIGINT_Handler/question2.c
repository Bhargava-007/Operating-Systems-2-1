#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

pid_t fg_pid = -1;

void handle_sigint(int sig) {
    if (fg_pid > 0) {
        printf("\n[Shell] Forwarding SIGINT to foreground %d\n", fg_pid);
        kill(fg_pid, SIGINT);
    } else {
        printf("\n[Shell] SIGINT caught, no foreground job\n");
    }
}

void run_fg(char *cmd) {
    fg_pid = fork();
    if (fg_pid == 0) {
        signal(SIGINT, SIG_DFL);
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args);
    }
    printf("[Shell] Foreground job %d: %s\n", fg_pid, cmd);
    int status;
    waitpid(fg_pid, &status, 0);
    if (WIFSIGNALED(status))
        printf("[Shell] Job %d terminated by signal %d\n", fg_pid, WTERMSIG(status));
    else
        printf("[Shell] Job %d done\n", fg_pid);
    fg_pid = -1;
}

int main() {
    printf("=== SIGINT Handler ===\n\n");

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    printf("Test 1: no foreground job\n");
    kill(getpid(), SIGINT);

    printf("\nTest 2: foreground job running\n");
    run_fg("echo job_output && sleep 1");

    printf("\nShell protected. Done.\n");
    return 0;
}
