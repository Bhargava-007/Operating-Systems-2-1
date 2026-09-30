#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void redirect_output(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return; }
    int saved = dup(1);
    dup2(fd, 1);
    close(fd);

    printf("line one\nline two\nline three\n");
    fflush(stdout);

    dup2(saved, 1);
    close(saved);
    printf("--- stdout restored ---\n");
}

void verify(char *file) {
    printf("--- Verifying %s ---\n", file);
    char line[128];
    FILE *f = fopen(file, "r");
    if (!f) { perror("fopen"); return; }
    while (fgets(line, sizeof(line), f))
        printf("%s", line);
    fclose(f);
}

int main() {
    printf("=== Output Redirection Demo ===\n\n");

    printf("Test 1: redirect to out.txt\n");
    redirect_output("out.txt");
    verify("out.txt");

    printf("\nTest 2: redirect to /root/noperm.txt\n");
    redirect_output("/root/noperm.txt");

    remove("out.txt");
    return 0;
}
