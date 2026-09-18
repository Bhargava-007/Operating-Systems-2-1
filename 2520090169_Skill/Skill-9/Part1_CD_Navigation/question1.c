#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 512

char prev_dir[MAX] = "";

int change_dir(const char *path) {
    char cwd[MAX];
    getcwd(cwd, sizeof(cwd));

    /* cd - goes to previous directory */
    if (strcmp(path, "-") == 0) {
        if (prev_dir[0] == '\0') { printf("cd: no previous directory\n"); return 1; }
        path = prev_dir;
    }

    if (chdir(path) != 0) { perror("cd"); return 1; }

    strcpy(prev_dir, cwd);
    getcwd(cwd, sizeof(cwd));
    printf("Changed to: %s\n", cwd);
    return 0;
}

int main() {
    char cwd[MAX];
    printf("========================================\n");
    printf("   CD Navigation Demo\n");
    printf("========================================\n\n");

    getcwd(cwd, sizeof(cwd));
    printf("Start: %s\n\n", cwd);

    printf("--- cd /tmp ---\n");
    change_dir("/tmp");

    printf("\n--- cd /var ---\n");
    change_dir("/var");

    printf("\n--- cd - (go back) ---\n");
    change_dir("-");

    printf("\n--- cd /root ---\n");
    change_dir("/root");

    printf("\n--- cd invalidpath ---\n");
    change_dir("/invalidpath");

    printf("\n--- cd ~ (HOME) ---\n");
    change_dir(getenv("HOME"));

    return 0;
}
