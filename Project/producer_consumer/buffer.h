#ifndef BUFFER_H
#define BUFFER_H

#define MAX_BUFFER 100

extern int buffer[MAX_BUFFER];
extern int in, out;
extern int BUFFER_SIZE;

void init_buffer();
int insert_item(int item);
int remove_item(int *item);

#endif
