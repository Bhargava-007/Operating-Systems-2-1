#include <stdio.h>
#include <time.h>
#include "logger.h"

static FILE *logfile = NULL;

void log_event(const char *msg) {
    if (!logfile) logfile = fopen("pc_log.txt", "a");
    if (logfile) {
        time_t t = time(NULL);
        fprintf(logfile, "[%ld] %s\n", t, msg);
        fflush(logfile);
    }
}

void close_log() {
    if (logfile) fclose(logfile);
}
