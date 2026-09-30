#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_STAGES 4
#define LEN 64

typedef struct {
    char cmds[MAX_STAGES][LEN];
    int count;
} Pipeline;

void add_stage(Pipeline *p, char *cmd) {
    if (p->count >= MAX_STAGES) { printf("Pipeline full\n"); return; }
    strncpy(p->cmds[p->count++], cmd, LEN);
    printf("Stage %d: %s\n", p->count, cmd);
}

void show_pipeline(Pipeline *p) {
    printf("\n--- Pipeline ---\n");
    for (int i = 0; i < p->count; i++) {
        printf("%s", p->cmds[i]);
        if (i < p->count-1) printf(" | ");
    }
    printf("\n");
}

void run_pipeline(Pipeline *p) {
    if (p->count < 2) { printf("Need at least 2 stages\n"); return; }
    printf("\nExecuting pipeline...\n");
    int pd[2]; pid_t pid;
    int in_fd = 0;
    for (int i = 0; i < p->count; i++) {
        pipe(pd);
        pid = fork();
        if (pid == 0) {
            dup2(in_fd, 0);
            if (i < p->count-1) dup2(pd[1], 1);
            close(pd[0]); close(pd[1]);
            char *args[] = {"sh", "-c", p->cmds[i], NULL};
            execvp("sh", args);
        }
        close(pd[1]);
        in_fd = pd[0];
    }
    wait(NULL);
}

int main() {
    Pipeline p = {.count = 0};
    printf("=== Pipeline Structure ===\n\n");
    add_stage(&p, "echo one two three");
    add_stage(&p, "tr ' ' '\\n'");
    add_stage(&p, "sort");
    show_pipeline(&p);
    run_pipeline(&p);
    return 0;
}
