#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

void do_export(char *arg) {
    char *eq = strchr(arg, '=');
    if (!eq) { printf("Invalid: use NAME=VALUE\n"); return; }
    *eq = 0;
    char *name = arg, *val = eq + 1;
    if (setenv(name, val, 1) == 0)
        printf("Exported: %s=%s\n", name, val);
    else
        perror("setenv");
}

void test_child(char *name) {
    char *v = getenv(name);
    if (!v) { printf("Not found: %s\n", name); return; }
    printf("Child sees %s=%s\n", name, v);
    char *args[] = {"env", NULL};
    execvp("env", args);
}

int main() {
    char input[256];
    printf("=== Export Shell ===\n");

    while (1) {
        printf("shell$ ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (!strcmp(input, "exit")) { printf("Bye!\n"); break; }
        else if (!strncmp(input, "export ", 7)) do_export(input + 7);
        else if (!strncmp(input, "test ", 5)) test_child(input + 5);
        else if (!strcmp(input, "env")) {
            for (char **e = environ; *e; e++) printf("%s\n", *e);
        } else printf("Unknown: %s\n", input);
    }
    return 0;
}
