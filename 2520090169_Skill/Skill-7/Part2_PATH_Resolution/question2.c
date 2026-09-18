#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 512

int find_executable(const char *cmd, char *full_path) {
    char *path_env = getenv("PATH");
    if (!path_env) { printf("PATH not set\n"); return 0; }

    char path_copy[MAX];
    strncpy(path_copy, path_env, MAX);

    char *dir = strtok(path_copy, ":");
    while (dir) {
        snprintf(full_path, MAX, "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0) return 1;
        dir = strtok(NULL, ":");
    }
    return 0;
}

int main() {
    printf("========================================\n");
    printf("   PATH Resolution Demo\n");
    printf("========================================\n\n");

    /* Print PATH directories */
    char *path_env = getenv("PATH");
    printf("PATH = %s\n\n", path_env);

    printf("--- Parsing PATH directories ---\n");
    char path_copy[MAX];
    strncpy(path_copy, path_env, MAX);
    char *dir = strtok(path_copy, ":");
    int n = 1;
    while (dir) { printf("  [%d] %s\n", n++, dir); dir = strtok(NULL, ":"); }

    /* Test command resolution */
    const char *cmds[] = {"ls", "gcc", "python3", "invalidcmd", "echo"};
    printf("\n--- Locating Executables ---\n");
    for (int i = 0; i < 5; i++) {
        char full_path[MAX] = {0};
        if (find_executable(cmds[i], full_path))
            printf("  [FOUND]   %-12s -> %s\n", cmds[i], full_path);
        else
            printf("  [MISSING] %-12s -> not found in PATH\n", cmds[i]);
    }

    return 0;
}
