#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create token structure / parse node */
ParseNode *create_node() {
    ParseNode *n = calloc(1, sizeof(ParseNode));
    if (!n) { perror("calloc"); exit(1); }
    return n;
}

/* Detect errors in token stream */
void detect_errors(const TokenStream *ts) {
    if (ts->count == 0) { printf("[Error] Empty token stream.\n"); return; }
    TokenType last = ts->tokens[0].type;
    for (int i = 1; i < ts->count; i++) {
        TokenType cur = ts->tokens[i].type;
        if (last == TOKEN_PIPE && cur == TOKEN_PIPE)
            printf("[Error] Consecutive pipes at index %d.\n", i);
        if (last == TOKEN_REDIRECT_OUT && cur != TOKEN_WORD)
            printf("[Error] Expected filename after '>' at index %d.\n", i);
        if (last == TOKEN_REDIRECT_IN && cur != TOKEN_WORD)
            printf("[Error] Expected filename after '<' at index %d.\n", i);
        last = cur;
    }
    if (ts->tokens[ts->count-1].type == TOKEN_PIPE)
        printf("[Error] Trailing pipe at end of command.\n");
}

/* Validate syntax */
int validate_syntax(const TokenStream *ts) {
    if (ts->count == 0) return 0;
    detect_errors(ts);
    return 1;
}

/* Handle empty commands */
void handle_empty_command(const char *input) {
    char clean[1024] = {0};
    handle_whitespace(input, clean);
    if (strlen(clean) == 0)
        printf("[Info] Empty command — nothing to execute.\n");
}

/* Generate parse tree from token stream */
ParseNode *generate_parse_tree(TokenStream *ts) {
    if (ts->count == 0) return NULL;

    ParseNode *root = create_node();
    ParseNode *cur  = root;
    int i = 0;

    while (i < ts->count) {
        Token *t = &ts->tokens[i];
        if (t->type == TOKEN_WORD) {
            if (strlen(cur->cmd) == 0) {
                strncpy(cur->cmd, t->value, MAX_TOKEN_LEN - 1);
            } else {
                strncpy(cur->args[cur->arg_count++], t->value, MAX_TOKEN_LEN - 1);
            }
        } else if (t->type == TOKEN_PIPE) {
            ParseNode *next = create_node();
            cur->pipe_to = next;
            cur = next;
        } else if (t->type == TOKEN_REDIRECT_IN && i+1 < ts->count) {
            strncpy(cur->infile, ts->tokens[++i].value, MAX_TOKEN_LEN - 1);
        } else if (t->type == TOKEN_REDIRECT_OUT && i+1 < ts->count) {
            strncpy(cur->outfile, ts->tokens[++i].value, MAX_TOKEN_LEN - 1);
            cur->append = 0;
        } else if (t->type == TOKEN_APPEND && i+1 < ts->count) {
            strncpy(cur->outfile, ts->tokens[++i].value, MAX_TOKEN_LEN - 1);
            cur->append = 1;
        }
        i++;
    }
    return root;
}

/* Produce execution structure */
void produce_execution_structure(const ParseNode *node) {
    printf("\n--- Execution Structure ---\n");
    int stage = 0;
    while (node) {
        printf("  Stage %d:\n", stage++);
        printf("    cmd     : '%s'\n", node->cmd);
        for (int i = 0; i < node->arg_count; i++)
            printf("    arg[%d]  : '%s'\n", i, node->args[i]);
        if (strlen(node->infile))  printf("    infile  : '%s'\n", node->infile);
        if (strlen(node->outfile)) printf("    outfile : '%s' (append=%d)\n", node->outfile, node->append);
        if (node->pipe_to) printf("    -> pipe to next stage\n");
        node = node->pipe_to;
    }
    printf("---------------------------\n");
}

/* Free parse tree */
void free_parse_tree(ParseNode *node) {
    while (node) {
        ParseNode *tmp = node->pipe_to;
        free(node);
        node = tmp;
    }
}
