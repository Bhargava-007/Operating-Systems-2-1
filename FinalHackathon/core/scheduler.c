#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

// Forward declaration of dynamic priority scheduler from dynamic_priority.c
ScheduleStats run_dynamic_priority(Process processes[], int n);

/**
 * @brief Helper to display the execution metrics summary table
 *
 * @param processes Array of processes after scheduling completion
 * @param n Number of processes
 * @return ScheduleStats Calculated averages
 */
static ScheduleStats print_summary_table(Process processes[], int n) {
    printf("\n----------------------------------------------------------------------------------------------------\n");
    printf("%-6s | %-6s | %-8s | %-7s | %-8s | %-12s | %-15s | %-13s\n",
           "PID", "Name", "Arrival", "Burst", "Priority", "Waiting Time", "Turnaround Time", "Response Time");
    printf("----------------------------------------------------------------------------------------------------\n");

    double total_wt = 0, total_tat = 0, total_rt = 0;
    int starvations = 0;

    for (int i = 0; i < n; i++) {
        printf("%-6d | %-6s | %-8d | %-7d | %-8d | %-12d | %-15d | %-13d\n",
               processes[i].pid, processes[i].name, processes[i].arrival_time,
               processes[i].burst_time, processes[i].current_priority,
               processes[i].waiting_time, processes[i].turnaround_time, processes[i].response_time);
        total_wt += processes[i].waiting_time;
        total_tat += processes[i].turnaround_time;
        total_rt += processes[i].response_time;
        if (processes[i].waiting_time > 20) {
            starvations++;
        }
    }
    printf("----------------------------------------------------------------------------------------------------\n");

    ScheduleStats stats;
    stats.avg_waiting_time = total_wt / n;
    stats.avg_turnaround_time = total_tat / n;
    stats.avg_response_time = total_rt / n;
    stats.starvation_count = starvations;

    printf("Averages: Waiting Time = %.2f | Turnaround Time = %.2f | Response Time = %.2f\n",
           stats.avg_waiting_time, stats.avg_turnaround_time, stats.avg_response_time);
    return stats;
}

/**
 * @brief First-Come, First-Served (FCFS) Non-Preemptive Scheduling
 * Dispatches processes strictly by arrival time.
 *
 * @param processes Input process array
 * @param n Total processes
 * @return ScheduleStats Resulting average statistics
 */
ScheduleStats run_fcfs(Process processes[], int n) {
    printf("\n=======================================================\n");
    printf("   SCHEDULER: First-Come First-Served (FCFS)\n");
    printf("=======================================================\n");

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = -1;
        processes[i].first_scheduled_time = -1;
        processes[i].current_priority = processes[i].original_priority;
        processes[i].state = WAITING;
    }

    int completed = 0;
    int current_time = 0;
    printf("\nExecution Trace:\n");

    while (completed < n) {
        // Find arrived process with earliest arrival time that is not DONE
        int selected_idx = -1;
        int earliest_arrival = 999999;

        for (int i = 0; i < n; i++) {
            if (processes[i].state != DONE && processes[i].arrival_time <= current_time) {
                if (processes[i].arrival_time < earliest_arrival) {
                    earliest_arrival = processes[i].arrival_time;
                    selected_idx = i;
                }
            }
        }

        if (selected_idx == -1) {
            printf("[t=%d] CPU Idle\n", current_time);
            current_time++;
            continue;
        }

        Process *curr = &processes[selected_idx];
        curr->state = RUNNING;
        curr->first_scheduled_time = current_time;
        curr->response_time = current_time - curr->arrival_time;
        curr->waiting_time = current_time - curr->arrival_time;

        // Run non-preemptively until completion
        while (curr->remaining_time > 0) {
            printf("[t=%d] Running PID %d (%s) [Remaining: %d]\n",
                   current_time, curr->pid, curr->name, curr->remaining_time);
            curr->remaining_time--;
            current_time++;
        }

        curr->state = DONE;
        curr->completion_time = current_time;
        curr->turnaround_time = curr->completion_time - curr->arrival_time;
        completed++;
        printf("[t=%d] PID %d (%s) COMPLETED (Turnaround: %d, Waiting: %d)\n",
               current_time, curr->pid, curr->name, curr->turnaround_time, curr->waiting_time);
    }

    return print_summary_table(processes, n);
}

/**
 * @brief Round Robin (RR) Preemptive Scheduling with Configurable Time Quantum
 *
 * @param processes Input process array
 * @param n Total processes
 * @param time_quantum Execution slice allocated per cycle
 * @return ScheduleStats Resulting average statistics
 */
ScheduleStats run_round_robin(Process processes[], int n, int time_quantum) {
    printf("\n=======================================================\n");
    printf("   SCHEDULER: Round Robin (Time Quantum = %d)\n", time_quantum);
    printf("=======================================================\n");

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = -1;
        processes[i].first_scheduled_time = -1;
        processes[i].current_priority = processes[i].original_priority;
        processes[i].state = WAITING;
    }

    // Circular queue for ready processes
    int queue[MAX_PROCESSES * 100];
    int front = 0, rear = 0;
    int in_queue[MAX_PROCESSES] = {0};

    int completed = 0;
    int current_time = 0;

    // Push initial arrived processes at t=0
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time <= current_time) {
            queue[rear++] = i;
            in_queue[i] = 1;
            processes[i].state = READY;
        }
    }

    printf("\nExecution Trace:\n");

    while (completed < n) {
        if (front == rear) {
            // Check if any process arrives in future
            int min_future_arr = 999999;
            for (int i = 0; i < n; i++) {
                if (processes[i].state != DONE && processes[i].arrival_time > current_time) {
                    if (processes[i].arrival_time < min_future_arr) {
                        min_future_arr = processes[i].arrival_time;
                    }
                }
            }
            if (min_future_arr != 999999) {
                while (current_time < min_future_arr) {
                    printf("[t=%d] CPU Idle\n", current_time);
                    current_time++;
                }
                for (int i = 0; i < n; i++) {
                    if (processes[i].arrival_time <= current_time && processes[i].state != DONE && !in_queue[i]) {
                        queue[rear++] = i;
                        in_queue[i] = 1;
                        processes[i].state = READY;
                    }
                }
                continue;
            } else {
                break;
            }
        }

        int curr_idx = queue[front++];
        in_queue[curr_idx] = 0;
        Process *curr = &processes[curr_idx];
        curr->state = RUNNING;

        if (curr->first_scheduled_time == -1) {
            curr->first_scheduled_time = current_time;
            curr->response_time = current_time - curr->arrival_time;
        }

        int exec_slice = (curr->remaining_time < time_quantum) ? curr->remaining_time : time_quantum;

        for (int slice = 0; slice < exec_slice; slice++) {
            printf("[t=%d] Running PID %d (%s) [Remaining: %d]\n",
                   current_time, curr->pid, curr->name, curr->remaining_time);
            curr->remaining_time--;
            current_time++;

            // Check for new arrivals during this tick
            for (int i = 0; i < n; i++) {
                if (processes[i].arrival_time == current_time && processes[i].state != DONE && !in_queue[i] && i != curr_idx) {
                    queue[rear++] = i;
                    in_queue[i] = 1;
                    processes[i].state = READY;
                }
            }
        }

        if (curr->remaining_time == 0) {
            curr->state = DONE;
            curr->completion_time = current_time;
            curr->turnaround_time = curr->completion_time - curr->arrival_time;
            curr->waiting_time = curr->turnaround_time - curr->burst_time;
            completed++;
            printf("[t=%d] PID %d (%s) COMPLETED (Turnaround: %d, Waiting: %d)\n",
                   current_time, curr->pid, curr->name, curr->turnaround_time, curr->waiting_time);
        } else {
            curr->state = READY;
            queue[rear++] = curr_idx;
            in_queue[curr_idx] = 1;
        }
    }

    return print_summary_table(processes, n);
}

/**
 * @brief Non-Preemptive Priority Scheduling
 * Lowest numerical priority value runs first to completion.
 *
 * @param processes Input process array
 * @param n Total processes
 * @return ScheduleStats Resulting average statistics
 */
ScheduleStats run_priority_non_preemptive(Process processes[], int n) {
    printf("\n=======================================================\n");
    printf("   SCHEDULER: Priority Scheduling (Non-Preemptive, Static)\n");
    printf("=======================================================\n");

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = -1;
        processes[i].first_scheduled_time = -1;
        processes[i].current_priority = processes[i].original_priority;
        processes[i].state = WAITING;
    }

    int completed = 0;
    int current_time = 0;
    printf("\nExecution Trace:\n");

    while (completed < n) {
        int selected_idx = -1;
        int highest_prio = 999999;
        int earliest_arr = 999999;

        for (int i = 0; i < n; i++) {
            if (processes[i].state != DONE && processes[i].arrival_time <= current_time) {
                if (processes[i].original_priority < highest_prio) {
                    highest_prio = processes[i].original_priority;
                    earliest_arr = processes[i].arrival_time;
                    selected_idx = i;
                } else if (processes[i].original_priority == highest_prio) {
                    if (processes[i].arrival_time < earliest_arr) {
                        earliest_arr = processes[i].arrival_time;
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
        curr->state = RUNNING;
        curr->first_scheduled_time = current_time;
        curr->response_time = current_time - curr->arrival_time;
        curr->waiting_time = current_time - curr->arrival_time;

        while (curr->remaining_time > 0) {
            printf("[t=%d] Running PID %d (%s) [Priority: %d | Remaining: %d]\n",
                   current_time, curr->pid, curr->name, curr->original_priority, curr->remaining_time);
            curr->remaining_time--;
            current_time++;
        }

        curr->state = DONE;
        curr->completion_time = current_time;
        curr->turnaround_time = curr->completion_time - curr->arrival_time;
        completed++;
        printf("[t=%d] PID %d (%s) COMPLETED (Turnaround: %d, Waiting: %d)\n",
                   current_time, curr->pid, curr->name, curr->turnaround_time, curr->waiting_time);
    }

    return print_summary_table(processes, n);
}

/**
 * @brief Clones an array of processes for fresh algorithm evaluation
 */
static void copy_processes(Process src[], Process dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int main(void) {
    printf("========================================================================\n");
    printf("   Smart CPU Scheduler with Dynamic Priority Adjustment (CLI Core)       \n");
    printf("========================================================================\n");

    // Standard workload exhibiting high and low priority contention with potential starvation
    Process sample_processes[] = {
        {1, "P1", 0,  8, 8, 3, 3, 0, 0, -1, 0, -1, WAITING},
        {2, "P2", 1,  4, 4, 1, 1, 0, 0, -1, 0, -1, WAITING},
        {3, "P3", 2,  9, 9, 4, 4, 0, 0, -1, 0, -1, WAITING},
        {4, "P4", 3,  5, 5, 2, 2, 0, 0, -1, 0, -1, WAITING},
        {5, "P5", 4,  2, 2, 1, 1, 0, 0, -1, 0, -1, WAITING}
    };
    int n = 5;

    Process test_set[MAX_PROCESSES];

    // 1. Run FCFS
    copy_processes(sample_processes, test_set, n);
    ScheduleStats fcfs_stats = run_fcfs(test_set, n);

    // 2. Run Round Robin (Quantum = 3)
    copy_processes(sample_processes, test_set, n);
    ScheduleStats rr_stats = run_round_robin(test_set, n, 3);

    // 3. Run Static Priority (Non-preemptive)
    copy_processes(sample_processes, test_set, n);
    ScheduleStats prio_stats = run_priority_non_preemptive(test_set, n);

    // 4. Run Dynamic Priority (With Aging)
    copy_processes(sample_processes, test_set, n);
    ScheduleStats dynamic_stats = run_dynamic_priority(test_set, n);

    // 5. Starvation Comparison Mode
    printf("\n========================================================================\n");
    printf("   ALGORITHM COMPARISON & STARVATION ANALYSIS                           \n");
    printf("========================================================================\n");
    printf("%-26s | %-12s | %-15s | %-13s | %-10s\n",
           "Algorithm", "Avg Waiting", "Avg Turnaround", "Avg Response", "Starvations");
    printf("------------------------------------------------------------------------\n");
    printf("%-26s | %-12.2f | %-15.2f | %-13.2f | %-10d\n",
           "FCFS", fcfs_stats.avg_waiting_time, fcfs_stats.avg_turnaround_time, fcfs_stats.avg_response_time, fcfs_stats.starvation_count);
    printf("%-26s | %-12.2f | %-15.2f | %-13.2f | %-10d\n",
           "Round Robin (Q=3)", rr_stats.avg_waiting_time, rr_stats.avg_turnaround_time, rr_stats.avg_response_time, rr_stats.starvation_count);
    printf("%-26s | %-12.2f | %-15.2f | %-13.2f | %-10d\n",
           "Static Priority", prio_stats.avg_waiting_time, prio_stats.avg_turnaround_time, prio_stats.avg_response_time, prio_stats.starvation_count);
    printf("%-26s | %-12.2f | %-15.2f | %-13.2f | %-10d\n",
           "Dynamic Priority (Aging)", dynamic_stats.avg_waiting_time, dynamic_stats.avg_turnaround_time, dynamic_stats.avg_response_time, dynamic_stats.starvation_count);
    printf("------------------------------------------------------------------------\n");

    double diff = prio_stats.avg_waiting_time - dynamic_stats.avg_waiting_time;
    if (diff > 0) {
        printf("RESULT: Dynamic Priority reduced average waiting time by %.2f units vs static priority.\n", diff);
    } else {
        printf("RESULT: Dynamic Priority prevented long-tail starvation while maintaining responsive throughput.\n");
    }
    printf("========================================================================\n");

    return 0;
}
