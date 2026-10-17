#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

void init_log(const char *session_id);
void log_event(const char *session_id, const char *role, int thread_id, const char *action, int item, int slot, int current_occupancy);
void close_log(void);

#endif /* LOGGER_H */
