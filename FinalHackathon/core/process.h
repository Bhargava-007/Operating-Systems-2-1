#ifndef PROCESS_H
#define PROCESS_H

/**
 * @file process.h
 * @brief Process definitions and scheduling structures for Smart CPU Scheduler
 */

#define MAX_PROCESSES 64
#define MAX_NAME_LEN 32

/**
 * @enum ProcessState
 * @brief States a process can inhabit during scheduling lifecycle
 */
typedef enum {
    READY,
    RUNNING,
    WAITING,
    DONE
} ProcessState;

/**
 * @struct Process
 * @brief Encapsulates PCB (Process Control Block) attributes
 * Note on priority convention: Lower numerical value indicates higher priority
 * e.g., Priority 1 is higher priority than Priority 5.
 */
typedef struct {
    int pid;                            /**< Unique Process Identifier */
    char name[MAX_NAME_LEN];            /**< Human readable label (e.g. P1, P2) */
    int arrival_time;                   /**< Timestamp when process enters ready queue */
    int burst_time;                     /**< Total CPU execution cycles required */
    int remaining_time;                 /**< Remaining CPU cycles to completion */
    int original_priority;              /**< Initial priority assigned at creation */
    int current_priority;               /**< Dynamically adjusted priority (via aging) */
    int waiting_time;                   /**< Total time units spent in ready queue */
    int turnaround_time;                /**< Total time from arrival to completion (completion_time - arrival_time) */
    int response_time;                  /**< Time elapsed from arrival until first CPU acquisition */
    int completion_time;                /**< Timestamp at which process reaches DONE */
    int first_scheduled_time;           /**< Timestamp when first dispatched (-1 if not yet run) */
    ProcessState state;                 /**< Current lifecycle state */
} Process;

/**
 * @struct ScheduleStats
 * @brief Aggregated scheduling outcome metrics
 */
typedef struct {
    double avg_waiting_time;
    double avg_turnaround_time;
    double avg_response_time;
    int starvation_count;
} ScheduleStats;

#endif /* PROCESS_H */
