#include <stdio.h>
#include <string.h>

int read_input(char *buffer, int size);

void run_interactive_loop(void)
{
    char buffer[1024];

    while (1) {
        printf("> ");
        fflush(stdout);

        read_input(buffer, 1024);

        if (strcmp(buffer, "help") == 0) {
            printf("Available commands: help, exit\n");
            continue;
        }

        if (strcmp(buffer, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        if (buffer[0] == '\0') {
            continue;
        }

        printf("You entered: %s\n", buffer);
    }
}