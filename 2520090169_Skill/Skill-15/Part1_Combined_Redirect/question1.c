#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void combined_redirect(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return; }
    int s1 = dup(1), s2 = dup(2);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);

    printf("stdout: normal output\n");
    fprintf(stderr, "stderr: error output\n");
    fflush(stdout); fflush(stderr);

    dup2(s1, 1); dup2(s2, 2);
    close(s1); close(s2);
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
    printf("=== Combined Redirection (stdout + stderr) ===\n\n");
    combined_redirect("combined.txt");
    verify("combined.txt");
    printf("\nBoth streams merged into one file.\n");
    remove("combined.txt");
    return 0;
}
