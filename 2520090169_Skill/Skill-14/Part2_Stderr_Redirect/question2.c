#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void redirect_stderr(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return; }
    int saved = dup(2);
    dup2(fd, 2);
    close(fd);
    fprintf(stderr, "error: file not found\n");
    fprintf(stderr, "error: permission denied\n");
    fflush(stderr);
    dup2(saved, 2);
    close(saved);
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
    printf("=== Stderr Redirection ===\n\n");
    printf("Redirecting stderr to err.txt...\n");
    redirect_stderr("err.txt");
    verify("err.txt");
    printf("\nTest: missing file error\n");
    FILE *f = fopen("nofile.txt", "r");
    if (!f) fprintf(stderr, "error: nofile.txt not found\n");
    remove("err.txt");
    return 0;
}
