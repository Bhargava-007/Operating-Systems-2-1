#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char saved_dir[256];

void save_state() { getcwd(saved_dir, sizeof(saved_dir)); }

int main() {
    char input[256], cwd[256];
    save_state();
    printf("Shell started. Saved dir: %s\n", saved_dir);

    while (1) {
        getcwd(cwd, sizeof(cwd));
        printf("shell:%s$ ", cwd);
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (!strcmp(input, "exit")) {
            printf("Saving state: %s\nCleaning up. Bye!\n", cwd);
            break;
        } else if (!strcmp(input, "pwd")) {
            printf("%s\n", cwd);
        } else if (!strncmp(input, "cd ", 3)) {
            if (chdir(input + 3)) perror("cd");
        } else {
            printf("Unknown: %s\n", input);
        }
    }
    return 0;
}
