# Linux Concurrent Producer-Consumer System Using Semaphores

**Course:** Operating Systems and Systems Programming (25CS2104E)  
**Term:** 2026-27, Term-I | Section 11 | Team 1

---

## Team Members
| Roll Number | Name     | Responsibility |
|-------------|----------|----------------|
| 2520090169  | Bhargava | System architecture, circular buffer, producer logic, integration testing |
| 2520090107  | Hanish   | Semaphore logic, consumer thread, GDB/strace debugging |
| 2520090162  | Teja     | Thread management, boundary testing, test case validation |
| 2520090043  | Pardhu   | Makefile, GitHub, performance analysis, code review |

---

## Description
A fully synchronized concurrent producer-consumer system implemented in C on Ubuntu Linux using POSIX semaphores and pthreads. Demonstrates mutual exclusion, process synchronization, bounded buffer management, and prevention of race conditions and deadlocks.

---

## Project Structure

    producer_consumer/
    ├── main.c          — argument parsing, thread creation/joining, summary
    ├── buffer.c        — circular buffer implementation
    ├── buffer.h        — buffer declarations
    ├── producer.c      — producer thread logic
    ├── producer.h      — producer declarations
    ├── consumer.c      — consumer thread logic
    ├── consumer.h      — consumer declarations
    ├── semaphores.c    — semaphore init and destroy
    ├── semaphores.h    — semaphore declarations
    └── Makefile        — build automation with object files
    screenshots/
    ├── 1_project_structure.png
    ├── 2_build_success.png
    ├── 3_basic_test.png
    ├── 4_boundary_test.png
    ├── 5_stress_test.png
    ├── 6_varying_buffer.png
    ├── 7_strace.png
    └── 8_gdb.png

---

## Semaphores Used
| Semaphore | Initial Value | Purpose |
|-----------|---------------|---------|
| empty     | buffer_size   | Tracks available slots — blocks producer when buffer is full |
| full      | 0             | Tracks filled slots — blocks consumer when buffer is empty |
| mutex     | 1             | Binary semaphore for mutual exclusion during buffer access |

---

## Build
    make

## Run
    ./pc <buffer_size> <num_producers> <num_consumers> <items_per_producer>

## Test Cases
| Test | Command | Purpose |
|------|---------|---------|
| Basic | `./pc 5 3 2 10` | Standard multi-producer multi-consumer run |
| Boundary | `./pc 3 1 1 5` | Single producer, single consumer |
| Stress | `./pc 10 5 5 20` | High load with many threads |
| Small Buffer | `./pc 2 4 4 8` | Forces producer blocking on full buffer |

## Clean
    make clean

---

## OS Concepts Demonstrated
- Bounded buffer with circular array
- POSIX semaphores (sem_init, sem_wait, sem_post, sem_destroy)
- POSIX threads (pthread_create, pthread_join)
- Mutual exclusion and critical section protection
- Deadlock prevention through correct semaphore ordering
- System call tracing with strace
- Multi-thread debugging with GDB

---

## GitHub Repository
[https://github.com/HanishDev0017/OS-Project](https://github.com/HanishDev0017/OS-Project)