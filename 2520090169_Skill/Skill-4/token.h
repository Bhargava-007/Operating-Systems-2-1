#ifndef TOKEN_H
#define TOKEN_H

#define MAX_TOKENS 64
#define MAX_TOKEN_LEN 256

typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_APPEND,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

typedef struct {
    Token tokens[MAX_TOKENS];
    int count;
} TokenStream;

/* Parse tree node */
typedef struct ParseNode {
    char cmd[MAX_TOKEN_LEN];
    char args[MAX_TOKENS][MAX_TOKEN_LEN];
    int arg_count;
    char infile[MAX_TOKEN_LEN];
    char outfile[MAX_TOKEN_LEN];
    int append;
    struct ParseNode *pipe_to;
} ParseNode;

/* tokenizer */
int  split_input(const char *input, TokenStream *ts);
void identify_delimiter(const char *tok, Token *t);
void handle_whitespace(const char *input, char *out);
void validate_token_stream(const TokenStream *ts);
void debug_token_stream(const TokenStream *ts);

/* parser */
ParseNode *create_node();
ParseNode *generate_parse_tree(TokenStream *ts);
int        validate_syntax(const TokenStream *ts);
void       detect_errors(const TokenStream *ts);
void       handle_empty_command(const char *input);
void       produce_execution_structure(const ParseNode *node);
void       free_parse_tree(ParseNode *node);

#endif
