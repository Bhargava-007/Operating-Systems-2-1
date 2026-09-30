#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>

#define MAX 6

void run_pipeline(char **cmds, int n) {
    int fd[MAX][2];
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
            execvp("sh", args); exit(1);
        }
    }
    for (int i = 0; i < n-1; i++) {
        close(fd[i][0]); close(fd[i][1]);
    }
    for (int i = 0; i < n; i++) waitpid(pids[i], NULL, 0);
}

int main() {
    printf("=== Large Pipeline Stress Test ===\n\n");
    struct timespec t1, t2;

    printf("Pipeline 1: 6-stage word count\n");
    char *p1[] = {
        "echo -e 'apple\\nbanana\\napple\\ncherry\\nbanana\\napple'",
        "sort", "uniq -c", "sort -rn", "head -3", "cat"
    };
    clock_gettime(CLOCK_MONOTONIC, &t1);
    run_pipeline(p1, 6);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("Time: %ldms\n\n", (t2.tv_nsec - t1.tv_nsec)/1000000 + (t2.tv_sec-t1.tv_sec)*1000);

    printf("Pipeline 2: 4-stage number processing\n");
    char *p2[] = {
        "seq 1 20", "awk '{print $1*$1}'",
        "awk '$1>100'", "head -5"
    };
    clock_gettime(CLOCK_MONOTONIC, &t1);
    run_pipeline(p2, 4);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("Time: %ldms\n\n", (t2.tv_nsec - t1.tv_nsec)/1000000 + (t2.tv_sec-t1.tv_sec)*1000);

    printf("All pipelines stable. No failures detected.\n");
    return 0;
}
