#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void redirect_input(char *file) {
    int fd = open(file, O_RDONLY);
    if (fd < 0) { perror("open"); return; }
    int saved = dup(0);
    dup2(fd, 0);
    close(fd);

    printf("--- Reading from %s ---\n", file);
    char line[128];
    while (fgets(line, sizeof(line), stdin))
        printf("%s", line);

    dup2(saved, 0);
    close(saved);
    printf("--- Stream restored ---\n");
}

int main() {
    printf("=== Input Redirection Demo ===\n\n");

    FILE *f = fopen("input.txt", "w");
    fprintf(f, "line one\nline two\nline three\n");
    fclose(f);

    printf("Test 1: valid file\n");
    redirect_input("input.txt");

    printf("\nTest 2: missing file\n");
    redirect_input("nofile.txt");

    remove("input.txt");
    return 0;
}
