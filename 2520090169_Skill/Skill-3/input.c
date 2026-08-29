#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Allocate buffer dynamically */
char *alloc_buffer(int *size) {
    *size = 64;
    char *buf = malloc(*size);
    if (!buf) { perror("malloc"); exit(1); }
    return buf;
}

/* Resize array - prevent buffer overflow */
char *resize_buffer(char *buf, int *size) {
    *size *= 2;
    char *tmp = realloc(buf, *size);
    if (!tmp) { perror("realloc"); free(buf); exit(1); }
    return tmp;
}

/* Manage linked list - store command in history */
void store_command(History *h, const char *cmd) {
    Node *node = malloc(sizeof(Node));
    if (!node) { perror("malloc node"); exit(1); }
    strncpy(node->cmd, cmd, MAX_CMD_LEN - 1);
    node->cmd[MAX_CMD_LEN - 1] = '\0';
    node->next = NULL;
    node->prev = h->tail;
    if (h->tail) h->tail->next = node;
    else h->head = node;
    h->tail = node;
    h->current = NULL;
    h->count++;
}

/* Navigate previous commands */
const char *navigate_prev(History *h) {
    if (!h->tail) return "";
    if (!h->current) h->current = h->tail;
    else if (h->current->prev) h->current = h->current->prev;
    return h->current->cmd;
}

/* Navigate next commands */
const char *navigate_next(History *h) {
    if (!h->current) return "";
    if (h->current->next) h->current = h->current->next;
    else { h->current = NULL; return ""; }
    return h->current->cmd;
}

/* Release memory correctly */
void free_history(History *h) {
    Node *n = h->head;
    while (n) {
        Node *tmp = n->next;
        free(n);
        n = tmp;
    }
    h->head = h->tail = h->current = NULL;
}
