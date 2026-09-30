#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX 4

typedef struct {
    int id; pid_t pid;
    char cmd[64], state[16];
} Job;

Job table[MAX];
int jcount = 0;
pid_t fg_pid = -1;

void handle_sigint(int sig) {
    if (fg_pid > 0) { kill(fg_pid, SIGINT); printf("\n[SIGINT] forwarded to %d\n", fg_pid); }
    else printf("\n[SIGINT] no foreground job\n");
}

void add_job(char *cmd, int bg) {
    pid_t pid = fork();
    if (pid == 0) {
        setpgid(0, 0);
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args); exit(1);
    }
    setpgid(pid, pid);
    table[jcount].id = jcount+1;
    table[jcount].pid = pid;
    snprintf(table[jcount].cmd,   64, "%s", cmd);
    snprintf(table[jcount].state, 16, bg ? "background" : "foreground");
    jcount++;
    if (!bg) {
        fg_pid = pid;
        printf("[fg] %d: %s\n", pid, cmd);
        waitpid(pid, NULL, 0);
        fg_pid = -1;
        snprintf(table[jcount-1].state, 16, "done");
    } else {
        printf("[bg] %d: %s\n", pid, cmd);
    }
}

void list_jobs() {
    printf("\n--- Jobs ---\n");
    for (int i = 0; i < jcount; i++)
        printf("[%d] %-8d %-12s %s\n", table[i].id, table[i].pid, table[i].state, table[i].cmd);
}

void redirect_run(char *cmd, char *outfile) {
    int fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd, 1); close(fd);
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args); exit(1);
    }
    close(fd);
    waitpid(pid, NULL, 0);
    printf("[redirect] %s > %s done\n", cmd, outfile);
}

int main() {
    printf("=== Module Integration ===\n\n");

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    printf("-- Foreground --\n");
    add_job("echo fg_job_output", 0);

    printf("\n-- Background --\n");
    add_job("sleep 1 && echo bg_done", 1);

    printf("\n-- Redirect --\n");
    redirect_run("echo redirected_output", "out.txt");

    list_jobs();
    sleep(2);
    remove("out.txt");
    printf("\nAll modules integrated successfully.\n");
    return 0;
}
