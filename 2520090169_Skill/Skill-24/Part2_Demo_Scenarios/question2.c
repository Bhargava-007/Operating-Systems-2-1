#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

pid_t fg_pid = -1;

void handle_sigint(int sig) {
    if (fg_pid > 0) { kill(fg_pid, SIGINT); printf("\n[SIGINT] forwarded to %d\n", fg_pid); }
    else printf("\n[SIGINT] no fg job\n");
}

void demo_pipeline() {
    printf("=== Demo 1: Pipeline ===\n");
    int fd[2]; pipe(fd);
    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(fd[1],1); close(fd[0]); close(fd[1]);
        execlp("echo","echo","one two three four five",NULL);
    }
    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(fd[0],0); close(fd[1]); close(fd[0]);
        execlp("wc","wc","-w",NULL);
    }
    close(fd[0]); close(fd[1]);
    waitpid(p1,NULL,0); waitpid(p2,NULL,0);
    printf("Pipeline: echo | wc -w -> done\n\n");
}

void demo_redirect() {
    printf("=== Demo 2: Redirect ===\n");
    int fd = open("demo.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd,1); close(fd);
        execlp("echo","echo","redirected output",NULL);
    }
    close(fd); waitpid(pid,NULL,0);
    system("cat demo.txt");
    remove("demo.txt");
    printf("Redirect: echo > demo.txt -> done\n\n");
}

void demo_signals() {
    printf("=== Demo 3: Signals ===\n");
    struct sigaction sa; memset(&sa,0,sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
    printf("Sending SIGINT to self...\n");
    kill(getpid(), SIGINT);
    printf("Shell survived SIGINT\n\n");
}

void demo_bg() {
    printf("=== Demo 4: Background Job ===\n");
    pid_t pid = fork();
    if (pid == 0) {
        char *a[]={"sh","-c","sleep 1 && echo bg_done",NULL};
        execvp("sh",a); exit(1);
    }
    printf("[bg] %d launched\n", pid);
    printf("Prompt returned immediately\n");
    waitpid(pid,NULL,0);
    printf("bg_job complete\n\n");
}

int main() {
    printf("=== Final Demo Scenarios ===\n\n");
    demo_pipeline();
    demo_redirect();
    demo_signals();
    demo_bg();
    printf("=== All Features Verified ===\n");
    return 0;
}
