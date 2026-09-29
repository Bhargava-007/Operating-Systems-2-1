# ForgeOS

A bare-metal OS simulation and functional Unix shell written in C.  
Built to explore memory management, CPU scheduling, and shell internals from scratch.

---

## Project Structure

```
ForgeOS/
├── NanoKernel/
│   ├── memory.c       # First-fit memory allocator
│   └── scheduler.c    # FCFS and Round Robin CPU scheduler
└── Shellforge/
    ├── main.c         # Shell REPL and signal handling
    └── shell.c        # Pipelines, redirection, builtins, history
```

---

## NanoKernel

### `memory.c` — Memory Manager

Simulates a 512 KB flat memory space divided into blocks.

**Features:**
- First-fit allocation with block splitting — if a free block is larger than requested, the remainder becomes its own free block
- Coalescing on deallocation — adjacent free blocks are merged to prevent fragmentation
- Visual memory map showing start address, size, PID, and status of every block

**Constants:**
| Macro | Value | Description |
|---|---|---|
| `MEMORY_SIZE` | 512 | Total memory in KB |
| `MAX_BLOCKS` | 20 | Maximum concurrent blocks |

**Key functions:**
| Function | Description |
|---|---|
| `init_memory()` | Initializes a single free block spanning all memory |
| `allocate(pid, size)` | First-fit allocation; splits block if leftover space exists |
| `deallocate(pid)` | Frees block by PID and merges adjacent free blocks |
| `display_memory()` | Prints a formatted memory map to stdout |

**Demo output:**
```
=== NanoKernel Memory Manager ===
[Memory] Initialized: 512 KB total

--- Memory Map ---
Start    Size     PID      Status
0        512      -1       FREE
------------------
[Memory] Allocated 100 KB to PID 1 at offset 0
[Memory] Allocated 200 KB to PID 2 at offset 100
[Memory] Allocated  50 KB to PID 3 at offset 300
```

---

### `scheduler.c` — Process Scheduler

Simulates CPU scheduling for 5 named processes using two classic algorithms.

**Algorithms:**

**FCFS (First Come First Served)**  
Non-preemptive. Processes run to completion in arrival order.  
Simple, no starvation, but long jobs block everything behind them.

**Round Robin (Quantum = 2)**  
Preemptive. Each process gets a fixed time slice before yielding to the next.  
Fair CPU distribution; good for interactive workloads.

**Metrics computed:** waiting time, turnaround time, and averages for both.

**Demo processes:**
| PID | Name | Burst Time |
|---|---|---|
| 1 | init | 5 |
| 2 | shell | 3 |
| 3 | editor | 8 |
| 4 | browser | 6 |
| 5 | daemon | 2 |

---

## Shellforge

A functional Unix shell. Not simulated — actually forks and execs real processes.

### `main.c` — REPL

- Reads input in a loop with a `[Shellforge]$` prompt
- Handles `SIGINT` (Ctrl+C) gracefully — reprints the prompt instead of terminating
- Sets `SIGCHLD` to `SIG_IGN` to auto-reap background child processes and prevent zombies
- Strips trailing newlines, skips empty input, logs each command to history

### `shell.c` — Core Shell Logic

**Builtins:**
| Command | Behaviour |
|---|---|
| `cd [dir]` | Change directory; defaults to `$HOME` |
| `pwd` | Print current working directory |
| `history` | Print numbered command history |
| `exit` | Print goodbye and exit with code 0 |

**I/O Redirection:**
| Operator | Behaviour |
|---|---|
| `>` | Redirect stdout to file (truncate) |
| `>>` | Redirect stdout to file (append) |
| `<` | Redirect stdin from file |

**Pipeline execution (`|`):**  
Splits the input on `|`, creates `n-1` pipes, forks one child per segment, wires `stdin`/`stdout` via `dup2`, closes all pipe ends in every child, and waits for all children. Handles up to `MAX_PIPES` segments.

**Background jobs (`&`):**  
Append `&` to any command to run it in the background. The shell prints the PID and returns to the prompt immediately without waiting.

**History:**  
Stores up to `MAX_HISTORY` entries in a circular buffer. Older entries are evicted when full.

**Usage examples:**
```bash
[Shellforge]$ ls -la
[Shellforge]$ cat file.txt | grep error | wc -l
[Shellforge]$ echo hello > out.txt
[Shellforge]$ sleep 10 &
[Shellforge]$ history
[Shellforge]$ cd /tmp && pwd
[Shellforge]$ exit
```

---

## Building

### NanoKernel

```bash
gcc -o memory    NanoKernel/memory.c    -Wall
gcc -o scheduler NanoKernel/scheduler.c -Wall
```

### Shellforge

```bash
gcc -o shellforge Shellforge/main.c Shellforge/shell.c -Wall
./shellforge
```

> Shellforge uses POSIX APIs (`fork`, `execvp`, `dup2`, `pipe`). Linux or macOS only.

---

## Limitations

- Memory manager is a flat array simulation, not real kernel memory
- Scheduler has no I/O burst modeling or priority levels
- Shell has no tab completion, arrow-key history navigation, or job control (`fg`/`bg` commands)
- Pipeline argument parser uses `strtok` which is destructive — commands with quoted spaces will break

---