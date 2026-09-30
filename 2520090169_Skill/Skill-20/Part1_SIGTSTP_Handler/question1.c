#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 4

typedef struct {
    int id;
    pid_t pid;
    char cmd[64];
    char state[16];
} Job;

Job table[MAX];
int jcount = 0;
pid_t fg_pid = -1;

void handle_sigtstp(int sig) {
    if (fg_pid > 0) {
        kill(fg_pid, SIGSTOP);
        for (int i = 0; i < jcount; i++)
            if (table[i].pid == fg_pid)
                snprintf(table[i].state, 16, "stopped");
        printf("\n[Shell] Job %d suspended\n", fg_pid);
        fg_pid = -1;
    }
}

void add_fg(char *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGTSTP, SIG_DFL);
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args);
    }
    table[jcount].id  = jcount + 1;
    table[jcount].pid = pid;
    snprintf(table[jcount].cmd,   64, "%s", cmd);
    snprintf(table[jcount].state, 16, "running");
    jcount++;
    fg_pid = pid;
    printf("[%d] %d running: %s\n", jcount, pid, cmd);
    waitpid(pid, NULL, WUNTRACED);
}

void list_jobs() {
    printf("\n--- Jobs ---\n");
    for (int i = 0; i < jcount; i++)
        printf("[%d] %-8d %-10s %s\n", table[i].id, table[i].pid, table[i].state, table[i].cmd);
}

int main() {
    printf("=== SIGTSTP Handler ===\n\n");

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigtstp;
    sigaction(SIGTSTP, &sa, NULL);

    add_fg("sleep 5");
    list_jobs();

    printf("\nSending SIGTSTP to foreground...\n");
    kill(getpid(), SIGTSTP);
    list_jobs();

    printf("\nResuming job...\n");
    kill(table[0].pid, SIGCONT);
    snprintf(table[0].state, 16, "running");
    list_jobs();

    kill(table[0].pid, SIGTERM);
    printf("\nDone.\n");
    return 0;
}
