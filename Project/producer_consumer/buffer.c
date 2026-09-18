#include <stdio.h>
#include "buffer.h"

int buffer[MAX_BUFFER];
int in = 0, out = 0;
int BUFFER_SIZE;

void init_buffer() {
    in = 0;
    out = 0;
    for (int i = 0; i < MAX_BUFFER; i++) buffer[i] = -1;
}

int insert_item(int item) {
    if (((in + 1) % BUFFER_SIZE) == out) return -1;
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    return 0;
}

int remove_item(int *item) {
    if (in == out) return -1;
    *item = buffer[out];
    buffer[out] = -1;
    out = (out + 1) % BUFFER_SIZE;
    return 0;
}
