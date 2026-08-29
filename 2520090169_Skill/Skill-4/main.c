#include "token.h"
#include <stdio.h>
#include <string.h>

int main() {
    printf("=== Skill-4: Tokenizer & Parser ===\n");

    const char *inputs[] = {
        "ls -la | grep src > out.txt",
        "cat < input.txt >> log.txt",
        "   echo   hello   world  ",
        "",
        "ls | | grep foo",
        "pwd"
    };
    int n = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < n; i++) {
        printf("\n========================================\n");
        printf("Input: '%s'\n", inputs[i]);

        handle_empty_command(inputs[i]);

        TokenStream ts = {0};
        split_input(inputs[i], &ts);
        debug_token_stream(&ts);
        validate_token_stream(&ts);

        if (validate_syntax(&ts)) {
            ParseNode *tree = generate_parse_tree(&ts);
            produce_execution_structure(tree);
            free_parse_tree(tree);
        }
    }
    return 0;
}
