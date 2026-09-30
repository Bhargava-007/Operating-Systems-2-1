#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 5
#define LEN 64

char history[MAX][LEN];
int count = 0;

void add(char *cmd) {
    if (count < MAX) {
        strncpy(history[count++], cmd, LEN);
    } else {
        for (int i = 1; i < MAX; i++)
            strcpy(history[i-1], history[i]);
        strncpy(history[MAX-1], cmd, LEN);
    }
    printf("Added: %s\n", cmd);
}

void show() {
    printf("\n--- History (%d/%d) ---\n", count, MAX);
    for (int i = 0; i < count; i++)
        printf("%d: %s\n", i+1, history[i]);
}

char *get(int i) {
    if (i < 1 || i > count) { printf("Invalid index\n"); return NULL; }
    return history[i-1];
}

int main() {
    char *cmds[] = {"ls","pwd","cd /tmp","echo hi","cat file","grep x"};
    printf("=== Command History (cap=%d) ===\n", MAX);
    for (int i = 0; i < 6; i++) add(cmds[i]);
    show();
    printf("\nRetrieve #2: %s\n", get(2));
    printf("Retrieve #5: %s\n", get(5));
    printf("Retrieve #9: ");  get(9);
    printf("Count: %d (consistent with cap %d)\n", count, MAX);
    return 0;
}
