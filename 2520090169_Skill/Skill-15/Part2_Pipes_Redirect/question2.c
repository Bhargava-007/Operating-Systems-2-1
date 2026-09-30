#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

void pipe_with_redirect(char *outfile) {
    int fd[2]; pipe(fd);
    int out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(fd[1], 1);
        close(fd[0]); close(fd[1]); close(out);
        execlp("echo", "echo", "hello from pipe", NULL);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(fd[0], 0);
        dup2(out, 1);
        close(fd[1]); close(fd[0]); close(out);
        execlp("tr", "tr", "a-z", "A-Z", NULL);
    }

    close(fd[0]); close(fd[1]); close(out);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

void verify(char *file) {
    char line[128];
    FILE *f = fopen(file, "r");
    if (!f) { perror("fopen"); return; }
    printf("--- %s contents ---\n", file);
    while (fgets(line, sizeof(line), f)) printf("%s", line);
    fclose(f);
}

int main() {
    printf("=== Pipe + Redirect ===\n");
    printf("Command: echo 'hello from pipe' | tr a-z A-Z > out.txt\n\n");
    pipe_with_redirect("out.txt");
    verify("out.txt");
    printf("\nPipe and redirection combined successfully.\n");
    remove("out.txt");
    return 0;
}
