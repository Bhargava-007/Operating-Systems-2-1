#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT    1024
#define MAX_ARGS     64
#define MAX_HISTORY  10
#define MAX_PIPES    10

void execute_command(char **args, int background);
void execute_pipeline(char *input);
void handle_redirection(char **args);
void add_history(char *cmd);
void show_history();
void handle_builtin(char **args);
int is_builtin(char *cmd);

#endif
