#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "shell.h"

// Ctrl+C just reprints prompt, doesn't kill shell
void sigint_handler(int sig) {
    printf("\n[Shellforge]$ ");
    fflush(stdout);
}

int main() {
    char input[MAX_INPUT];

    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, SIG_IGN); // auto-reap background children

    printf("=== Shellforge ===\n");
    printf("Type 'exit' to quit\n\n");

    while (1) {
        printf("[Shellforge]$ ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin))
            break;

        // strip newline
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        add_history(input);

        // make a copy for pipeline (strtok modifies string)
        char copy[MAX_INPUT];
        strncpy(copy, input, MAX_INPUT);

        execute_pipeline(copy);
    }

    return 0;
}
