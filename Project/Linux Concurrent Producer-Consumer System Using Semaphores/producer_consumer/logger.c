#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "logger.h"

static FILE *logfile = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_log(const char *session_id) {
    pthread_mutex_lock(&log_mutex);
    if (!logfile) {
        logfile = fopen("pc_log.txt", "a");
    }
    if (logfile) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        fprintf(logfile, "# SESSION_START session=%s timestamp_sec=%ld timestamp_ns=%ld\n",
                session_id ? session_id : "default", ts.tv_sec, ts.tv_nsec);
        fflush(logfile);
    }
    pthread_mutex_unlock(&log_mutex);
}

void log_event(const char *session_id, const char *role, int thread_id, const char *action, int item, int slot, int current_occupancy) {
    pthread_mutex_lock(&log_mutex);
    if (!logfile) {
        logfile = fopen("pc_log.txt", "a");
    }
    if (logfile) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        // Structured machine-parseable and human-readable format:
        // [TIME_SEC.TIME_NS] [session] [ROLE ID] action item=X slot=Y occupancy=Z
        fprintf(logfile, "[%ld.%09ld] [%s] [%s %d] %s item=%d slot=%d occupancy=%d\n",
                ts.tv_sec, ts.tv_nsec,
                session_id ? session_id : "default",
                role, thread_id, action, item, slot, current_occupancy);
        fflush(logfile);
    }
    pthread_mutex_unlock(&log_mutex);
}

void close_log(void) {
    pthread_mutex_lock(&log_mutex);
    if (logfile) {
        fclose(logfile);
        logfile = NULL;
    }
    pthread_mutex_unlock(&log_mutex);
}
