#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void append_to(char *file, char *msg) {
    int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) { perror("open"); return; }
    int saved = dup(1);
    dup2(fd, 1);
    close(fd);
    printf("%s\n", msg);
    fflush(stdout);
    dup2(saved, 1);
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
    printf("=== Append Redirection ===\n\n");
    append_to("out.txt", "first line");
    append_to("out.txt", "second line");
    append_to("out.txt", "third line");
    verify("out.txt");
    printf("\nExisting data preserved across appends.\n");
    remove("out.txt");
    return 0;
}
