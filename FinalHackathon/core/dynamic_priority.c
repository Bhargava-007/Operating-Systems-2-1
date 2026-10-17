#include <stdio.h>
#include <stdlib.h>
#include "process.h"

#define AGING_INTERVAL 5
#define MAX_PRIORITY_BOOST 5
#define STARVATION_THRESHOLD 20

/**
 * @brief Adjusts the current priority of waiting processes based on elapsed waiting time.
 * Formula: Every AGING_INTERVAL (5) time units waited, priority boosts by 1.
 * Capped at MAX_PRIORITY_BOOST (5) levels above original priority.
 *
 * Lower numerical priority value represents higher scheduling urgency.
 * Priority boost: current_priority = max(1, original_priority - boost)
 *
 * @param p Pointer to the Process struct being inspected
 * @param current_time Global discrete timeline unit
 */
void apply_aging(Process *p, int current_time) {
    if (p == NULL || p->state == DONE) return;

    int boost = p->waiting_time / AGING_INTERVAL;
    if (boost > MAX_PRIORITY_BOOST) {
        boost = MAX_PRIORITY_BOOST;
    }

    int old_priority = p->current_priority;
    int calculated_priority = p->original_priority - boost;
    if (calculated_priority < 1) {
        calculated_priority = 1; // Highest allowable priority ceiling
    }

    if (calculated_priority != old_priority) {
        p->current_priority = calculated_priority;
        printf("[t=%d] PID %d (%s): priority boosted from %d -> %d (waited %d units)\n",
               current_time, p->pid, p->name, old_priority, p->current_priority, p->waiting_time);
    }

    if (p->waiting_time > STARVATION_THRESHOLD) {
        printf("  [!] STARVATION WARNING: PID %d (%s) has waited %d units (> %d threshold)\n",
               p->pid, p->name, p->waiting_time, STARVATION_THRESHOLD);
    }
}

/**
 * @brief Scans all ready processes at current timestep and applies aging
 *
 * @param processes Array of Process structs
 * @param n Total count of processes
 * @param current_time Global discrete timestep
 */
void age_waiting_processes(Process processes[], int n, int current_time) {
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time <= current_time && processes[i].state == READY) {
            apply_aging(&processes[i], current_time);
        }
    }
}

/**
 * @brief Dynamic priority scheduling with starvation prevention.
 * Operates preemptively or tick-by-tick, re-evaluating highest priority at each time unit.
 *
 * @param processes Array of input processes
 * @param n Total number of processes
 * @return ScheduleStats Average metrics calculated
 */
ScheduleStats run_dynamic_priority(Process processes[], int n) {
    printf("\n=======================================================\n");
    printf("   SCHEDULER: Dynamic Priority with Aging & Starvation Detection\n");
    printf("   (Aging interval: %d units | Boost cap: +%d | Starvation: >%d units)\n",
           AGING_INTERVAL, MAX_PRIORITY_BOOST, STARVATION_THRESHOLD);
    printf("=======================================================\n");

    int completed = 0;
    int current_time = 0;
    int starvation_warnings = 0;

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = -1;
        processes[i].first_scheduled_time = -1;
        processes[i].current_priority = processes[i].original_priority;
        processes[i].state = (processes[i].arrival_time == 0) ? READY : WAITING;
    }

    printf("\nExecution Trace:\n");

    while (completed < n) {
        // 1. Mark newly arrived processes as READY
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].state == WAITING) {
                processes[i].state = READY;
            }
        }

        // 2. Select process in READY state with highest priority (lowest numerical value)
        int selected_idx = -1;
        int best_priority = 999999;
        int earliest_arrival = 999999;

        for (int i = 0; i < n; i++) {
            if (processes[i].state == READY && processes[i].remaining_time > 0) {
                if (processes[i].current_priority < best_priority) {
                    best_priority = processes[i].current_priority;
                    earliest_arrival = processes[i].arrival_time;
                    selected_idx = i;
                } else if (processes[i].current_priority == best_priority) {
                    // Tie-breaker: FCFS arrival time
                    if (processes[i].arrival_time < earliest_arrival) {
                        earliest_arrival = processes[i].arrival_time;
                        selected_idx = i;
                    }
                }
            }
        }

        if (selected_idx == -1) {
            printf("[t=%d] CPU Idle\n", current_time);
            current_time++;
            continue;
        }

        Process *curr = &processes[selected_idx];

        // Record first response time
        if (curr->first_scheduled_time == -1) {
            curr->first_scheduled_time = current_time;
            curr->response_time = current_time - curr->arrival_time;
        }

        printf("[t=%d] Running PID %d (%s) [Priority: %d | Remaining: %d]\n",
               current_time, curr->pid, curr->name, curr->current_priority, curr->remaining_time);

        // Advance 1 time unit
        curr->remaining_time--;
        current_time++;

        // Update waiting time for all OTHER processes in READY state
        for (int i = 0; i < n; i++) {
            if (i != selected_idx && processes[i].state == READY && processes[i].remaining_time > 0) {
                processes[i].waiting_time++;
                // Apply dynamic aging
                apply_aging(&processes[i], current_time);
                if (processes[i].waiting_time > STARVATION_THRESHOLD) {
                    starvation_warnings++;
                }
            }
        }

        // Check if current process completed
        if (curr->remaining_time == 0) {
            curr->state = DONE;
            curr->completion_time = current_time;
            curr->turnaround_time = curr->completion_time - curr->arrival_time;
            completed++;
            printf("[t=%d] PID %d (%s) COMPLETED (Turnaround: %d, Waiting: %d)\n",
                   current_time, curr->pid, curr->name, curr->turnaround_time, curr->waiting_time);
        }
    }

    // Print summary table
    printf("\n----------------------------------------------------------------------------------------------------\n");
    printf("%-6s | %-6s | %-8s | %-7s | %-8s | %-12s | %-15s | %-13s\n",
           "PID", "Name", "Arrival", "Burst", "Priority", "Waiting Time", "Turnaround Time", "Response Time");
    printf("----------------------------------------------------------------------------------------------------\n");

    double total_wt = 0, total_tat = 0, total_rt = 0;
    for (int i = 0; i < n; i++) {
        printf("%-6d | %-6s | %-8d | %-7d | %-8d | %-12d | %-15d | %-13d\n",
               processes[i].pid, processes[i].name, processes[i].arrival_time,
               processes[i].burst_time, processes[i].current_priority,
               processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time);
        total_wt += processes[i].waiting_time;
        total_tat += processes[i].turnaround_time;
        total_rt += processes[i].response_time;
    }
    printf("----------------------------------------------------------------------------------------------------\n");

    ScheduleStats stats;
    stats.avg_waiting_time = total_wt / n;
    stats.avg_turnaround_time = total_tat / n;
    stats.avg_response_time = total_rt / n;
    stats.starvation_count = starvation_warnings;

    printf("Averages: Waiting Time = %.2f | Turnaround Time = %.2f | Response Time = %.2f\n",
           stats.avg_waiting_time, stats.avg_turnaround_time, stats.avg_response_time);
    return stats;
}
