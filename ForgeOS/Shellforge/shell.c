#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"

// ── History ──────────────────────────────────────────────
static char *history[MAX_HISTORY];
static int   history_count = 0;

void add_history(char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++)
            history[i-1] = history[i];
        history[MAX_HISTORY-1] = strdup(cmd);
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++)
        printf("  %d  %s\n", i+1, history[i]);
}

// ── Built-ins ─────────────────────────────────────────────
int is_builtin(char *cmd) {
    return (!strcmp(cmd,"cd") || !strcmp(cmd,"pwd") ||
            !strcmp(cmd,"exit") || !strcmp(cmd,"history"));
}

void handle_builtin(char **args) {
    if (!strcmp(args[0], "cd")) {
        char *dir = args[1] ? args[1] : getenv("HOME");
        if (chdir(dir) != 0)
            perror("cd");
    } else if (!strcmp(args[0], "pwd")) {
        char cwd[MAX_INPUT];
        if (getcwd(cwd, sizeof(cwd)))
            printf("%s\n", cwd);
    } else if (!strcmp(args[0], "history")) {
        show_history();
    } else if (!strcmp(args[0], "exit")) {
        printf("Goodbye!\n");
        exit(0);
    }
}

// ── Redirection ───────────────────────────────────────────
void handle_redirection(char **args) {
    for (int i = 0; args[i]; i++) {
        if (!strcmp(args[i], ">")) {
            int fd = open(args[i+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        } else if (!strcmp(args[i], ">>")) {
            int fd = open(args[i+1], O_WRONLY|O_CREAT|O_APPEND, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        } else if (!strcmp(args[i], "<")) {
            int fd = open(args[i+1], O_RDONLY);
            if (fd < 0) { perror("open"); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
        }
    }
}

// ── Single command ────────────────────────────────────────
void execute_command(char **args, int background) {
    if (!args[0]) return;

    if (is_builtin(args[0])) {
        handle_builtin(args);
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        handle_redirection(args);
        execvp(args[0], args);
        perror(args[0]);
        exit(1);
    } else if (pid > 0) {
        if (!background)
            waitpid(pid, NULL, 0);
        else
            printf("[bg] PID %d\n", pid);
    } else {
        perror("fork");
    }
}

// ── Pipeline ──────────────────────────────────────────────
void execute_pipeline(char *input) {
    char *segments[MAX_PIPES];
    int   num = 0;

    // split on '|'
    char *token = strtok(input, "|");
    while (token && num < MAX_PIPES) {
        segments[num++] = token;
        token = strtok(NULL, "|");
    }

    if (num == 1) {
        // no pipe — parse args normally
        char *args[MAX_ARGS];
        int   argc = 0, bg = 0;
        char *t = strtok(segments[0], " \t\n");
        while (t) {
            args[argc++] = t;
            t = strtok(NULL, " \t\n");
        }
        if (argc > 0 && !strcmp(args[argc-1], "&")) {
            bg = 1;
            args[--argc] = NULL;
        }
        args[argc] = NULL;
        execute_command(args, bg);
        return;
    }

    // multiple pipes
    int pipes[MAX_PIPES-1][2];
    for (int i = 0; i < num-1; i++)
        pipe(pipes[i]);

    for (int i = 0; i < num; i++) {
        char *args[MAX_ARGS];
        int   argc = 0;
        char  seg[MAX_INPUT];
        strncpy(seg, segments[i], MAX_INPUT);
        char *t = strtok(seg, " \t\n");
        while (t) { args[argc++] = t; t = strtok(NULL," \t\n"); }
        args[argc] = NULL;
        if (!args[0]) continue;

        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0)
                dup2(pipes[i-1][0], STDIN_FILENO);
            if (i < num-1)
                dup2(pipes[i][1], STDOUT_FILENO);
            for (int j = 0; j < num-1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(args[0], args);
            perror(args[0]);
            exit(1);
        }
    }

    for (int i = 0; i < num-1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < num; i++)
        wait(NULL);
}
