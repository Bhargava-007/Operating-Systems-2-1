#ifndef BUFFER_H
#define BUFFER_H

#define MAX_BUFFER 100

extern int buffer[MAX_BUFFER];
extern int in;
extern int out;
extern int count;
extern int BUFFER_SIZE;

void init_buffer(void);
int insert_item(int item);
int remove_item(int *item);

#endif /* BUFFER_H */
