#ifndef PRODUCER_H
#define PRODUCER_H

typedef struct {
    int id;
    int items_to_produce;
    const char *session_id;
} producer_arg_t;

void *producer(void *arg);

#endif /* PRODUCER_H */
