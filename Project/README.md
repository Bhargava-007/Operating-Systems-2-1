# Linux Concurrent Producer-Consumer System Using Semaphores

OS Project | 25CS2104E | Section 11 | Team 1

## Team Members
| Roll Number   | Name     |
|---------------|----------|
| 2520090169    | Bhargava |
| 2520090107    | Hanish   |
| 2520090162    | Teja     |
| 2520090043    | Pardhu   |

## Description
A concurrent producer-consumer system implemented in C on Linux using POSIX semaphores and pthreads. Demonstrates mutual exclusion, process synchronization, and bounded buffer management.

## Files
- producer_consumer.c  — main implementation
- Makefile             — build automation
- README.md            — this file

## Build
    make

## Run
    ./pc <buffer_size> <num_producers> <num_consumers> <items_per_producer>

## Examples
    ./pc 5 3 2 10     # basic test
    ./pc 3 1 1 5      # boundary test
    ./pc 10 5 5 20    # stress test
    ./pc 2 4 4 8      # small buffer, many threads

## Semaphores Used
- empty  : tracks available buffer slots (init = buffer_size)
- full   : tracks filled buffer slots (init = 0)
- mutex  : binary semaphore for mutual exclusion (init = 1)
