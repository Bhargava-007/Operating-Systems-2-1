#include <stdio.h>
#include <string.h>

#define MAX 256

/* Extract content between single quotes */
int extract_single_quoted(const char *input, char *output) {
    int i = 0, j = 0;
    int in_quote = 0;
    while (input[i]) {
        if (input[i] == '\'' && !in_quote) { in_quote = 1; i++; continue; }
        if (input[i] == '\'' &&  in_quote) { in_quote = 0; i++; continue; }
        if (in_quote) output[j++] = input[i];
        i++;
    }
    output[j] = '\0';
    return j;
}

int main() {
    /* Test cases */
    const char *tests[] = {
        "'Hello World'",
        "'$HOME is not expanded'",
        "'it'\\''s an edge case'",
        "'spaces   preserved'",
        "'123 !@# special'"
    };

    printf("========================================\n");
    printf("   Single Quote Parsing Demo\n");
    printf("========================================\n");

    for (int i = 0; i < 5; i++) {
        char result[MAX] = {0};
        extract_single_quoted(tests[i], result);
        printf("\nInput  : %s\n", tests[i]);
        printf("Parsed : %s\n", result);
        printf("Length : %lu\n", strlen(result));
    }

    /* Variable expansion test - single quotes prevent it */
    char *var = "EXPANDED";
    printf("\n--- Variable Expansion Test ---\n");
    printf("Single quoted  : '$var' -> literal, no expansion\n");
    printf("Without quotes : %s -> expanded\n", var);

    return 0;
}
