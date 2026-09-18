#include <stdio.h>
#include <string.h>

#define MAX 256

void parse_escaped(const char *input, char *output) {
    int i = 0, j = 0;
    while (input[i]) {
        if (input[i] == '\\' && input[i+1]) {
            i++;
            switch (input[i]) {
                case 'n':  output[j++] = '\n'; break;
                case 't':  output[j++] = '\t'; break;
                case ' ':  output[j++] = ' ';  break;
                case '\\': output[j++] = '\\'; break;
                case '$':  output[j++] = '$';  break;
                case '\'': output[j++] = '\''; break;
                case '"':  output[j++] = '"';  break;
                default:   output[j++] = '\\'; output[j++] = input[i]; break;
            }
        } else {
            output[j++] = input[i];
        }
        i++;
    }
    output[j] = '\0';
}

int main() {
    const char *tests[] = {
        "Hello\\ World",
        "price\\ is\\ \\$100",
        "tab\\there",
        "it\\'s\\ working",
        "back\\\\slash",
        "email\\@domain\\.com"
    };

    printf("========================================\n");
    printf("   Escape Sequence Parser Demo\n");
    printf("========================================\n");

    for (int i = 0; i < 6; i++) {
        char result[MAX] = {0};
        parse_escaped(tests[i], result);
        printf("\nInput  : %s\n", tests[i]);
        printf("Parsed : %s\n", result);
    }

    return 0;
}
