#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 256

/* Extract content between double quotes, handle variable expansion */
void extract_double_quoted(const char *input, char *output) {
    int i = 0, j = 0, in_quote = 0;
    while (input[i]) {
        if (input[i] == '"' && !in_quote) { in_quote = 1; i++; continue; }
        if (input[i] == '"' &&  in_quote) { in_quote = 0; i++; continue; }
        if (in_quote) {
            /* Simulate variable expansion for $VAR */
            if (input[i] == '$') {
                char var[64] = {0};
                int k = 0;
                i++;
                while (input[i] && input[i] != ' ' && input[i] != '"')
                    var[k++] = input[i++];
                char *val = getenv(var);
                if (val) { strcpy(output + j, val); j += strlen(val); }
                else     { strcpy(output + j, "(unset)"); j += 7; }
                continue;
            }
            output[j++] = input[i];
        }
        i++;
    }
    output[j] = '\0';
}

int main() {
    setenv("NAME", "Linux", 1);
    setenv("OS", "Ubuntu", 1);

    const char *tests[] = {
        "\"Hello World\"",
        "\"Value of NAME is $NAME\"",
        "\"OS is $OS and version is $VERSION\"",
        "\"  spaces   preserved  \"",
        "\"nested 'single' inside double\""
    };

    printf("========================================\n");
    printf("   Double Quote Parsing Demo\n");
    printf("========================================\n");

    for (int i = 0; i < 5; i++) {
        char result[MAX] = {0};
        extract_double_quoted(tests[i], result);
        printf("\nInput  : %s\n", tests[i]);
        printf("Parsed : %s\n", result);
    }

    printf("\n--- Comparison ---\n");
    printf("Single quotes : no expansion  -> '$NAME' = $NAME\n");
    printf("Double quotes : with expansion -> \"$NAME\" = %s\n", getenv("NAME"));

    return 0;
}
