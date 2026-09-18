#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 512

void expand_variables(const char *input, char *output) {
    int i = 0, j = 0;
    while (input[i]) {
        if (input[i] == '$') {
            i++;
            char var[64] = {0};
            int k = 0;
            while (input[i] && input[i] != ' ' && input[i] != '$')
                var[k++] = input[i++];
            char *val = getenv(var);
            if (val) { strcpy(output + j, val); j += strlen(val); }
            else     { strcpy(output + j, "(undefined)"); j += 11; }
        } else {
            output[j++] = input[i++];
        }
    }
    output[j] = '\0';
}

int main() {
    setenv("NAME", "Linux", 1);
    setenv("SHELL", "myshell", 1);
    setenv("USER", "bhargav", 1);

    const char *tests[] = {
        "$NAME is awesome",
        "Running $SHELL on $NAME",
        "Hello $USER welcome to $NAME",
        "$UNDEFINED variable test",
        "$NAME $SHELL $USER"
    };

    printf("========================================\n");
    printf("   Variable Expansion Demo\n");
    printf("========================================\n\n");

    for (int i = 0; i < 5; i++) {
        char result[MAX] = {0};
        expand_variables(tests[i], result);
        printf("Input  : %s\n", tests[i]);
        printf("Output : %s\n\n", result);
    }

    return 0;
}
