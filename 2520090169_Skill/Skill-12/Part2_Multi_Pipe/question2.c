#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX 4

void run_pipeline(char **cmds, int n) {
    int fd[MAX-1][2];
    pid_t pids[MAX];

    for (int i = 0; i < n-1; i++) pipe(fd[i]);

    for (int i = 0; i < n; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            if (i > 0)   dup2(fd[i-1][0], 0);
            if (i < n-1) dup2(fd[i][1], 1);
            for (int j = 0; j < n-1; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
            }
            char *args[] = {"sh", "-c", cmds[i], NULL};
            execvp("sh", args);
            exit(1);
        }
    }

    for (int i = 0; i < n-1; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }
    for (int i = 0; i < n; i++) waitpid(pids[i], NULL, 0);
}

int main() {
    printf("=== Multi-Pipe Pipeline ===\n");

    printf("\nPipeline 1: echo | tr | sort | uniq\n");
    char *p1[] = {"echo -e 'b\\na\\nc\\na\\nb'", "tr '\\n' '\\n'", "sort", "uniq"};
    run_pipeline(p1, 4);

    printf("\nPipeline 2: ls | grep .c | sort | head -3\n");
    char *p2[] = {"ls", "grep .c", "sort", "head -3"};
    run_pipeline(p2, 4);

    printf("\nAll pipelines complete. Resources cleaned up.\n");
    return 0;
}
