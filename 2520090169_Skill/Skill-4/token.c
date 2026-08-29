#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Handle whitespace - strip leading/trailing, collapse middle */
void handle_whitespace(const char *input, char *out) {
    int i = 0, j = 0;
    while (isspace((unsigned char)input[i])) i++;
    while (input[i]) {
        if (isspace((unsigned char)input[i])) {
            out[j++] = ' ';
            while (isspace((unsigned char)input[i])) i++;
        } else {
            out[j++] = input[i++];
        }
    }
    if (j > 0 && out[j-1] == ' ') j--;
    out[j] = '\0';
}

/* Identify delimiters and set token type */
void identify_delimiter(const char *tok, Token *t) {
    strncpy(t->value, tok, MAX_TOKEN_LEN - 1);
    t->value[MAX_TOKEN_LEN - 1] = '\0';
    if (strcmp(tok, "|")  == 0) t->type = TOKEN_PIPE;
    else if (strcmp(tok, "<")  == 0) t->type = TOKEN_REDIRECT_IN;
    else if (strcmp(tok, ">>") == 0) t->type = TOKEN_APPEND;
    else if (strcmp(tok, ">")  == 0) t->type = TOKEN_REDIRECT_OUT;
    else t->type = TOKEN_WORD;
}

/* Split input into tokens */
int split_input(const char *input, TokenStream *ts) {
    ts->count = 0;
    char clean[1024];
    handle_whitespace(input, clean);

    char buf[1024];
    strncpy(buf, clean, sizeof(buf) - 1);
    buf[sizeof(buf)-1] = '\0';

    char *p = buf;
    while (*p && ts->count < MAX_TOKENS) {
        /* skip spaces */
        while (*p == ' ') p++;
        if (!*p) break;

        char tok[MAX_TOKEN_LEN];
        int k = 0;

        /* two-char operators */
        if (p[0] == '>' && p[1] == '>') {
            strcpy(tok, ">>"); p += 2;
        } else if (*p == '|' || *p == '<' || *p == '>') {
            tok[0] = *p++; tok[1] = '\0';
        } else {
            /* word */
            while (*p && *p != ' ' && *p != '|' && *p != '<' && *p != '>')
                tok[k++] = *p++;
            tok[k] = '\0';
        }
        identify_delimiter(tok, &ts->tokens[ts->count++]);
    }
    return ts->count;
}

/* Validate token stream */
void validate_token_stream(const TokenStream *ts) {
    printf("[Validate] %d token(s) found.\n", ts->count);
    for (int i = 0; i < ts->count; i++) {
        if (ts->tokens[i].type == TOKEN_UNKNOWN)
            printf("[Validate] Unknown token at index %d: '%s'\n", i, ts->tokens[i].value);
    }
}

/* Debug parsing output */
void debug_token_stream(const TokenStream *ts) {
    printf("\n--- Token Stream Debug ---\n");
    const char *names[] = {"WORD","PIPE","REDIR_IN","REDIR_OUT","APPEND","EOF","UNKNOWN"};
    for (int i = 0; i < ts->count; i++)
        printf("  [%d] type=%-10s value='%s'\n", i, names[ts->tokens[i].type], ts->tokens[i].value);
    printf("--------------------------\n");
}
