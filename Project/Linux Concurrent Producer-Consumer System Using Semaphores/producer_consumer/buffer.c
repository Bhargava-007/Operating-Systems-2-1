#include <stdio.h>
#include "buffer.h"

int buffer[MAX_BUFFER];
int in = 0, out = 0;
int count = 0;
int BUFFER_SIZE;

void init_buffer(void) {
    in = 0;
    out = 0;
    count = 0;
    for (int i = 0; i < MAX_BUFFER; i++) {
        buffer[i] = -1;
    }
}

int insert_item(int item) {
    if (count >= BUFFER_SIZE) {
        return -1; // Buffer full invariant violation
    }
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    count++;
    return 0;
}

int remove_item(int *item) {
    if (count <= 0) {
        return -1; // Buffer empty invariant violation
    }
    *item = buffer[out];
    buffer[out] = -1;
    out = (out + 1) % BUFFER_SIZE;
    count--;
    return 0;
}
