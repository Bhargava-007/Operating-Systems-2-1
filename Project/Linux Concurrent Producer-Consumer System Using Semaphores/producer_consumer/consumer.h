#ifndef CONSUMER_H
#define CONSUMER_H

typedef struct {
    int id;
    int items_to_consume;
    const char *session_id;
} consumer_arg_t;

void *consumer(void *arg);

#endif /* CONSUMER_H */
