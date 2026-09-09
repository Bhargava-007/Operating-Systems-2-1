#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10

typedef struct {
    int pid;
    char name[20];
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int arrival_time;
} Process;

// ── FCFS ─────────────────────────────────────────────────
void fcfs(Process procs[], int n) {
    printf("\n=== FCFS Scheduling ===\n");
    printf("%-6s %-12s %-10s %-10s %-12s\n",
           "PID","Name","Burst","Waiting","Turnaround");

    int time = 0;
    for (int i = 0; i < n; i++) {
        procs[i].waiting_time    = time;
        time                    += procs[i].burst_time;
        procs[i].turnaround_time = time;

        printf("%-6d %-12s %-10d %-10d %-12d\n",
               procs[i].pid,
               procs[i].name,
               procs[i].burst_time,
               procs[i].waiting_time,
               procs[i].turnaround_time);
    }

    float avg_wait = 0, avg_tat = 0;
    for (int i = 0; i < n; i++) {
        avg_wait += procs[i].waiting_time;
        avg_tat  += procs[i].turnaround_time;
    }
    printf("\nAvg Waiting Time   : %.2f\n", avg_wait / n);
    printf("Avg Turnaround Time: %.2f\n",  avg_tat  / n);
}

// ── Round Robin ───────────────────────────────────────────
void round_robin(Process procs[], int n, int quantum) {
    printf("\n=== Round Robin Scheduling (Quantum=%d) ===\n", quantum);

    // reset remaining times
    for (int i = 0; i < n; i++)
        procs[i].remaining_time = procs[i].burst_time;

    int time = 0, done = 0;
    int wait[MAX_PROCESSES] = {0};

    while (done < n) {
        int all_done = 1;
        for (int i = 0; i < n; i++) {
            if (procs[i].remaining_time > 0) {
                all_done = 0;
                int run = (procs[i].remaining_time > quantum)
                          ? quantum : procs[i].remaining_time;

                printf("[Time %2d] Running PID %d (%s) for %d units\n",
                       time, procs[i].pid, procs[i].name, run);

                procs[i].remaining_time -= run;
                time                   += run;

                if (procs[i].remaining_time == 0) {
                    procs[i].turnaround_time = time;
                    procs[i].waiting_time    = time - procs[i].burst_time;
                    done++;
                }
            }
        }
        if (all_done) break;
    }

    printf("\n%-6s %-12s %-10s %-10s %-12s\n",
           "PID","Name","Burst","Waiting","Turnaround");
    float avg_wait = 0, avg_tat = 0;
    for (int i = 0; i < n; i++) {
        printf("%-6d %-12s %-10d %-10d %-12d\n",
               procs[i].pid,
               procs[i].name,
               procs[i].burst_time,
               procs[i].waiting_time,
               procs[i].turnaround_time);
        avg_wait += procs[i].waiting_time;
        avg_tat  += procs[i].turnaround_time;
    }
    printf("\nAvg Waiting Time   : %.2f\n", avg_wait / n);
    printf("Avg Turnaround Time: %.2f\n",  avg_tat  / n);
}

int main() {
    Process procs[MAX_PROCESSES] = {
        {1, "init",    5, 5, 0, 0, 0},
        {2, "shell",   3, 3, 0, 0, 0},
        {3, "editor",  8, 8, 0, 0, 0},
        {4, "browser", 6, 6, 0, 0, 0},
        {5, "daemon",  2, 2, 0, 0, 0},
    };
    int n = 5;

    printf("=== NanoKernel Process Scheduler ===\n");
    printf("Processes: %d\n", n);

    fcfs(procs, n);

    // reset for RR
    Process procs2[MAX_PROCESSES];
    memcpy(procs2, procs, sizeof(procs));
    round_robin(procs2, n, 2);

    return 0;
}
