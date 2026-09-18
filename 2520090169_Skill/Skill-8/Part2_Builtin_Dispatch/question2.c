#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 256

typedef int (*builtin_fn)(char **args);

int builtin_cd(char **args) {
    if (!args[1]) { fprintf(stderr, "cd: missing argument\n"); return 1; }
    if (chdir(args[1]) != 0) { perror("cd"); return 1; }
    printf("Changed directory to: %s\n", args[1]);
    return 0;
}

int builtin_pwd(char **args) {
    char cwd[MAX];
    if (getcwd(cwd, sizeof(cwd))) printf("%s\n", cwd);
    else perror("pwd");
    return 0;
}

int builtin_echo(char **args) {
    for (int i = 1; args[i]; i++)
        printf("%s%s", args[i], args[i+1] ? " " : "\n");
    return 0;
}

int builtin_exit(char **args) {
    int code = args[1] ? atoi(args[1]) : 0;
    printf("Exiting with code %d\n", code);
    exit(code);
}

int builtin_env(char **args) {
    printf("NAME=%s\n", getenv("NAME") ? getenv("NAME") : "(unset)");
    printf("HOME=%s\n", getenv("HOME") ? getenv("HOME") : "(unset)");
    printf("PATH=%s\n", getenv("PATH") ? getenv("PATH") : "(unset)");
    return 0;
}

/* Dispatch table */
struct { const char *name; builtin_fn fn; } builtins[] = {
    {"cd",   builtin_cd},
    {"pwd",  builtin_pwd},
    {"echo", builtin_echo},
    {"exit", builtin_exit},
    {"env",  builtin_env},
    {NULL, NULL}
};

int dispatch(char **args) {
    for (int i = 0; builtins[i].name; i++)
        if (strcmp(args[0], builtins[i].name) == 0)
            return builtins[i].fn(args);
    printf("'%s': not a builtin command\n", args[0]);
    return 1;
}

int main() {
    printf("========================================\n");
    printf("   Builtin Command Dispatch Demo\n");
    printf("========================================\n\n");

    char *cmd1[] = {"pwd",  NULL};
    char *cmd2[] = {"echo", "Hello", "from", "builtin!", NULL};
    char *cmd3[] = {"cd",   "/tmp", NULL};
    char *cmd4[] = {"pwd",  NULL};
    char *cmd5[] = {"env",  NULL};
    char *cmd6[] = {"invalid", NULL};

    printf("--- pwd ---\n");        dispatch(cmd1);
    printf("\n--- echo ---\n");     dispatch(cmd2);
    printf("\n--- cd /tmp ---\n");  dispatch(cmd3);
    printf("\n--- pwd ---\n");      dispatch(cmd4);
    printf("\n--- env ---\n");      dispatch(cmd5);
    printf("\n--- invalid ---\n");  dispatch(cmd6);

    return 0;
}
