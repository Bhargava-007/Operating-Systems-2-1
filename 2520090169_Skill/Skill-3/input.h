#ifndef INPUT_H
#define INPUT_H

#define MAX_HISTORY 100
#define MAX_CMD_LEN 256

typedef struct Node {
    char cmd[MAX_CMD_LEN];
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    Node *current;
    int count;
} History;

char *alloc_buffer(int *size);
char *resize_buffer(char *buf, int *size);
void store_command(History *h, const char *cmd);
const char *navigate_prev(History *h);
const char *navigate_next(History *h);
void free_history(History *h);
void run_input_loop();

#endif
