#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 256

typedef int (*builtin_fn)(char **args);

int builtin_cd(char **args) {
    if (!args[1]) { chdir(getenv("HOME")); printf("cd: went to HOME\n"); return 0; }
    if (chdir(args[1]) != 0) { perror("cd"); return 1; }
    char cwd[MAX]; getcwd(cwd, sizeof(cwd));
    printf("cd: now in %s\n", cwd);
    return 0;
}

int builtin_pwd(char **args) {
    char cwd[MAX];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    return 0;
}

int builtin_echo(char **args) {
    for (int i = 1; args[i]; i++)
        printf("%s%s", args[i], args[i+1] ? " " : "\n");
    return 0;
}

int builtin_export(char **args) {
    if (!args[1]) { printf("export: missing argument\n"); return 1; }
    char *eq = strchr(args[1], '=');
    if (!eq) { printf("export: use VAR=value format\n"); return 1; }
    *eq = '\0';
    setenv(args[1], eq+1, 1);
    printf("Exported: %s=%s\n", args[1], eq+1);
    return 0;
}

int builtin_unset(char **args) {
    if (!args[1]) { printf("unset: missing argument\n"); return 1; }
    unsetenv(args[1]);
    printf("Unset: %s\n", args[1]);
    return 0;
}

struct { const char *name; builtin_fn fn; } builtins[] = {
    {"cd",     builtin_cd},
    {"pwd",    builtin_pwd},
    {"echo",   builtin_echo},
    {"export", builtin_export},
    {"unset",  builtin_unset},
    {NULL, NULL}
};

int dispatch(char **args) {
    for (int i = 0; builtins[i].name; i++)
        if (strcmp(args[0], builtins[i].name) == 0)
            return builtins[i].fn(args);
    printf("'%s': not a builtin\n", args[0]);
    return 1;
}

int main() {
    printf("========================================\n");
    printf("   Builtin Dispatch Demo\n");
    printf("========================================\n\n");

    char *cmd1[] = {"pwd", NULL};
    char *cmd2[] = {"cd", "/tmp", NULL};
    char *cmd3[] = {"pwd", NULL};
    char *cmd4[] = {"echo", "Hello", "Skill-9!", NULL};
    char *cmd5[] = {"export", "MYVAR=HelloOS", NULL};
    char *cmd6[] = {"echo", getenv("HOME"), NULL};
    char *cmd7[] = {"unset", "MYVAR", NULL};
    char *cmd8[] = {"invalid", NULL};

    printf("--- pwd ---\n");         dispatch(cmd1);
    printf("\n--- cd /tmp ---\n");   dispatch(cmd2);
    printf("\n--- pwd ---\n");       dispatch(cmd3);
    printf("\n--- echo ---\n");      dispatch(cmd4);
    printf("\n--- export ---\n");    dispatch(cmd5);
    printf("\n--- echo HOME ---\n"); dispatch(cmd6);
    printf("\n--- unset ---\n");     dispatch(cmd7);
    printf("\n--- invalid ---\n");   dispatch(cmd8);

    return 0;
}
