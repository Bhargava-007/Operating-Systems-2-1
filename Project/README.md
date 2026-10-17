# Linux Concurrent Producer-Consumer System Using Semaphores

**Course:** Operating Systems and Systems Programming — 25CS2104E  
**Institution:** KL Deemed to be University, Department of CSE-1  
**Term:** 2026-27, Odd Semester (Term-I) | Section: 11 | Team: 1  
**Faculty:** Mrs. Alekhya  
**GitHub:** [https://github.com/HanishDev0017/OS-Project](https://github.com/HanishDev0017/OS-Project)

---

## Team Members

| Roll Number | Name     | Individual Responsibility |
|-------------|----------|---------------------------|
| 2520090169  | Bhargava | System architecture, circular buffer implementation, producer thread logic, integration testing |
| 2520090107  | Hanish   | Semaphore logic, consumer thread, GDB/strace debugging, project report and demonstration |
| 2520090162  | Teja     | Thread management, boundary testing, test case validation |
| 2520090043  | Pardhu   | Makefile, GitHub, performance analysis, code review |

---

## Abstract

This project implements a **Linux Concurrent Producer-Consumer System** using POSIX semaphores and POSIX threads (pthreads) as the primary synchronization and concurrency mechanism. The producer-consumer problem is a classical operating systems challenge: multiple concurrent threads share a bounded buffer where producers generate data and consumers retrieve it. Without proper synchronization, race conditions, buffer overflows, and deadlocks occur.

The solution implements a fixed-size shared circular buffer managed by multiple producer and consumer threads running concurrently on Ubuntu Linux. Three POSIX semaphores are employed — `empty` to track available slots, `full` to track filled slots, and `mutex` as a binary semaphore for mutual exclusion during critical section access. File I/O logging via `logger.c` records all events to `pc_log.txt`, and system call tracing using `strace` demonstrates the user-space to kernel-space boundary. The implementation is written in C using Linux/POSIX APIs and validated through boundary, concurrent, and stress test scenarios.

---

## Problem Statement

In multi-threaded environments, concurrent access to shared resources is a fundamental OS design challenge. The producer-consumer problem specifically addresses scenarios where producers generate data into a shared bounded buffer while consumers retrieve and process it. The core challenge is ensuring:

- Producers do not write to a **full** buffer
- Consumers do not read from an **empty** buffer
- No two threads access the shared buffer simultaneously in an unsafe manner

Without proper synchronization primitives, these violations lead to race conditions, data corruption, and non-deterministic behavior. This project addresses the problem by implementing a correctly synchronized system on Linux using POSIX semaphores, directly demonstrating the practical application of OS synchronization theory in systems programming.

---

## Course Outcome Mapping

| CO | Course Outcome | Coverage in This Project | Files / Tools |
|----|----------------|--------------------------|---------------|
| **CO1** | Analyze the OS as a layered service abstraction — kernel mode vs user mode, syscall interface | Semaphore operations (`sem_wait`, `sem_post`) and thread management calls are kernel-mediated system calls, crossing the user-space to kernel-space boundary. Traced using `strace` to observe this at the syscall level. CO1 comment block in `main.c` documents this mapping. | `main.c`, `strace` (`7_strace.png`) |
| **CO2** | Apply Unix process-control primitives — lifecycle and state transitions | Thread lifecycle mirrors process lifecycle: Created (`pthread_create`) → Running → Blocked on `sem_wait` → Ready on `sem_post` → Terminated (`pthread_join`, analogous to `wait`). Thread state transitions documented in comments in producer and consumer logic. | `producer.c`, `consumer.c`, `main.c`, GDB (`8_gdb.png`) |
| **CO3** | Apply IPC mechanisms — shared memory, synchronization | The shared circular buffer is the inter-thread shared memory IPC medium. Producers write into and consumers read from this shared structure, coordinated by three semaphores — directly demonstrating shared memory IPC and synchronization. | `buffer.c`, `buffer.h`, `semaphores.c` |
| **CO4** | Analyze virtual memory — address space layout, dynamic allocation | The circular buffer (`buffer[]`) and global semaphore objects reside in the data/BSS segment of the process address space. Each POSIX thread receives its own stack segment allocated by the Linux kernel within the process virtual address space. | `buffer.c`, `main.c`, GDB (`8_gdb.png`) |
| **CO5** | Analyze file-system abstractions and use file-I/O syscalls | `logger.c` implements Linux file I/O using `fopen`, `fprintf`, `fflush`, and `fclose` to write timestamped producer and consumer events to `pc_log.txt`. Demonstrates the Unix file abstraction, file descriptor management, and buffered I/O backed by kernel syscalls (`open`, `write`, `close`). | `logger.c`, `logger.h`, `pc_log.txt` |
| **CO6** | Analyze concurrency primitives — threads, semaphores, deadlocks | Primary CO. Full implementation of POSIX threads (`pthread_create`, `pthread_join`), counting semaphores (`empty`, `full`), binary semaphore as mutex, critical section protection, and deadlock-free design through correct semaphore acquisition ordering. | `semaphores.c`, `producer.c`, `consumer.c`, `main.c` |

---

## Semaphore Design

| Semaphore | Type     | Initial Value | Role |
|-----------|----------|---------------|------|
| `empty`   | Counting | `buffer_size` | Tracks available slots — producer calls `sem_wait(&empty)` and blocks when buffer is full |
| `full`    | Counting | `0`           | Tracks occupied slots — consumer calls `sem_wait(&full)` and blocks when buffer is empty |
| `mutex`   | Binary   | `1`           | Enforces mutual exclusion — both threads acquire this before entering the critical section |

**Acquisition order** (identical for producer and consumer) prevents deadlock:
```
sem_wait(resource semaphore)  →  sem_wait(mutex)  →  [critical section]  →  sem_post(mutex)  →  sem_post(signal semaphore)
```

---

## Project Structure

```
OS-Project/
├── Docs/
│   └── Abstract.pdf
├── Linux Concurrent Producer-Consumer System Using Semaphores/
│   ├── producer_consumer/
│   │   ├── main.c          — entry point: argument parsing, thread creation, monotonic timing, metrics
│   │   ├── buffer.c        — circular buffer: insert_item(), remove_item(), init_buffer()
│   │   ├── buffer.h        — buffer declarations, capacity constants, and count tracker
│   │   ├── producer.c      — producer thread function with safe_sem_wait & invariant logging
│   │   ├── producer.h      — producer declarations and argument struct
│   │   ├── consumer.c      — consumer thread function with safe_sem_wait & invariant logging
│   │   ├── consumer.h      — consumer declarations and argument struct
│   │   ├── semaphores.c    — init_semaphores(), destroy_semaphores(), safe_sem_wait() (EINTR loop)
│   │   ├── semaphores.h    — semaphore declarations (sem_t empty, full, mutex)
│   │   ├── logger.c        — thread-safe structured file I/O logging with session IDs [CO5]
│   │   ├── logger.h        — logger declarations and function prototypes
│   │   ├── Makefile        — build automation with -Wall -Wextra -pthread -O2
│   │   └── run_tests.sh    — automated timeout-based concurrency & deadlock test suite
│   └── screenshots/        — verification output and debugging screenshots
├── dashboard/              — Sage Green Systems-Engineering Monitoring UI
│   ├── server.js           — Node.js backend bridge with 15s deadlock watchdog
│   ├── package.json        — server configuration
│   └── public/
│       ├── index.html      — responsive structured layout
│       ├── style.css       — minimalist Sage Green aesthetic system
│       └── app.js          — interactive circular buffer SVG & native event playback
└── README.md
```

---

## OS Concepts and Linux APIs Used

| OS Concept / Linux API | Purpose in Project | CO |
|------------------------|--------------------|----|
| `sem_init`, `sem_wait`, `sem_post`, `sem_destroy` | Counting and binary semaphore synchronization | CO6 |
| `pthread_create`, `pthread_join`, `pthread_exit` | Thread creation, lifecycle, termination | CO2, CO6 |
| Shared circular buffer (global array) | Inter-thread shared memory IPC | CO3, CO4 |
| `fopen`, `fprintf`, `fflush`, `fclose` | Buffered file I/O for event logging | CO5 |
| `strace` | Syscall tracing to observe kernel-user space transitions | CO1 |
| `GDB` | Multi-threaded debugging and thread state inspection | CO2, CO6 |
| Process virtual address space (data, stack segments) | Thread stacks and global buffer in virtual memory | CO4 |
| Critical section and mutual exclusion | Binary semaphore protecting shared buffer access | CO6 |
| Deadlock prevention via semaphore ordering | Consistent acquisition order eliminates circular wait | CO6 |

---

## Deadlock Analysis

| Deadlock Condition | Status in This Project |
|--------------------|------------------------|
| Mutual Exclusion | Required and intentional — `mutex` semaphore enforces it |
| Hold and Wait | Eliminated — `mutex` is acquired only after the resource semaphore (`empty`/`full`) |
| No Preemption | N/A — semaphores are released explicitly after every critical section |
| Circular Wait | Eliminated — all threads follow identical semaphore acquisition order |

---

## Build and Run

### Prerequisites
- Ubuntu Linux or WSL
- `gcc` with pthread support
- `make`

### Build
```bash
cd producer_consumer
make
```

### Run
```bash
./pc <buffer_size> <num_producers> <num_consumers> <items_per_producer>
```

### Arguments
| Argument | Description | Valid Range |
|----------|-------------|-------------|
| `buffer_size` | Size of the shared circular buffer | 1 – 100 |
| `num_producers` | Number of producer threads | ≥ 1 |
| `num_consumers` | Number of consumer threads | ≥ 1 |
| `items_per_producer` | Items each producer will produce | ≥ 1 |

### Clean
```bash
make clean
```

---

## Test Cases

| Test | Command | What It Tests |
|------|---------|---------------|
| Basic | `./pc 5 3 2 10` | Standard multi-producer multi-consumer concurrent run |
| Boundary | `./pc 3 1 1 5` | Minimal config — single producer and consumer |
| Stress | `./pc 10 5 5 20` | High thread count and item volume — stability under load |
| Small Buffer | `./pc 2 4 4 8` | Tiny buffer forces producers to block — exercises `sem_wait(&empty)` |
| Default (`make run`) | `./pc 5 2 2 4` | Quick smoke test via Makefile |

---

---

## Verified Execution & Invariant Reporting

```
[Producer 1] Produced: 932 at slot 0 (occupancy: 1/5)
[Producer 2] Produced: 237 at slot 1 (occupancy: 2/5)
[Consumer 1] Consumed: 932 from slot 0 (occupancy: 1/5)
[Consumer 2] Consumed: 237 from slot 1 (occupancy: 0/5)
...

--- Execution Metrics & Invariant Report ---
Session ID               : web_m3a9f0
Buffer Capacity          : 5
Producer Threads         : 2 (total produced: 8)
Consumer Threads         : 2 (total consumed: 8)
Execution Duration       : 55.634 ms (0.056 s)
Final Buffer Occupancy   : 0 items
Final Semaphore Invariants: empty=5/5, full=0/0, mutex=1/1
Verification Status      : PASS (All synchronization invariants satisfied, 0 deadlocks observed)
```

All events are simultaneously recorded with nanosecond timestamps, thread roles, item values, and session IDs to `pc_log.txt` via thread-safe file I/O (CO5):
```
[1792212337.962095029] [session_1] [Producer 1] INSERT item=876 slot=2 occupancy=1
[1792212337.965837678] [session_1] [Consumer 1] REMOVE item=876 slot=2 occupancy=1
```

---

## Automated Concurrency Test Suite

An automated timeout-based test suite (`run_tests.sh`) validates synchronization under diverse configurations:
- **Boundary Buffer Size 1**: Strictly serialized handoff between producer and consumer.
- **Large Buffer Size 20**: High-capacity loose coupling.
- **High Producer Contention (6P:1C)**: Exercises `sem_wait(&empty)` blocking under heavy buffer-full pressure.
- **High Consumer Contention (1P:6C)**: Exercises `sem_wait(&full)` blocking under buffer-empty starvation.
- **Stress Concurrency (5P:5C, Buffer 8, 100 items)**: 10 concurrent threads under sustained interleaving.

Run the test suite:
```bash
./run_tests.sh
```

---

## System Call Trace (CO1)

```bash
strace ./pc 5 2 2 4
```

Key syscalls observed: `futex` (backing `sem_wait`/`sem_post`), `clone` (thread creation), `mmap` (thread stack allocation), `openat`/`write`/`close` (file logging). Demonstrates the user-space to kernel-space transition described in CO1. See `screenshots/7_strace.png`.