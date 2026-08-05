# Q4 - Process Synchronization using `wait()` and `waitpid()`

## Aim

To develop a C program where a parent process creates multiple child processes and synchronizes their execution using `wait()` and `waitpid()`. The experiment also compares the behavior of both synchronization functions.

---

## Requirements

- Linux Operating System
- GCC Compiler
- Terminal

---

## Files

```text
Q4/
├── wait_waitpid.c
├── README.md
└── screenshots/
    ├── 01_program_output.png
    ├── 02_ps_processes.png
    ├── 03_top_processes.png
    ├── 04_proc_status.png
    └── 05_process_terminated.png
```

---

## Algorithm

1. Start the program.
2. Create two child processes using `fork()`.
3. The first child sleeps for a fixed duration and exits.
4. The second child sleeps for a longer duration and exits.
5. The parent waits specifically for the first child using `waitpid()`.
6. The parent waits for the remaining child using `wait()`.
7. Observe the processes using Linux monitoring tools (`ps`, `top`, and `/proc`).
8. After both child processes terminate, the parent process exits.

---

## Source Code

See `wait_waitpid.c`.

---

## Compilation

```bash
gcc wait_waitpid.c -o wait_waitpid
```

---

## Execution

```bash
./wait_waitpid
```

---

## Sample Output

```text
Child 1: PID = xxxx
Parent PID: xxxx
Waiting specifically for Child 1 using waitpid()...
Child 2: PID = xxxx
Child 1 exiting.
Child 1 finished.
Waiting for remaining child using wait()...
Child 2 exiting.
Child 2 finished.
Parent exiting.
```

---

## Commands Used

### Display Process Information

```bash
ps -ef | grep wait_waitpid
```

### Monitor Processes

```bash
top
```

### View Process Details

```bash
cat /proc/<child_pid>/status
```

---

## Observations

- One parent process created two child processes using `fork()`.
- `waitpid()` waited specifically for the first child process to terminate.
- `wait()` waited for the remaining child process.
- During execution, both child processes entered the **Sleeping (S)** state while executing `sleep()`.
- The `/proc/<PID>/status` file displayed the process details, including PID, PPID, and current process state.
- After both child processes completed execution, the parent process exited successfully.
- The final `ps` command confirmed that no `wait_waitpid` processes remained.

---

## Comparison of `wait()` and `waitpid()`

| `wait()` | `waitpid()` |
|----------|-------------|
| Waits for any child process to terminate. | Waits for a specific child process identified by its PID. |
| Simpler to use when any child can be collected. | Provides greater control by allowing synchronization with a selected child process. |
| Returns when the first child exits. | Returns only after the specified child exits. |

---

## Conclusion

The experiment successfully demonstrated process synchronization using both `wait()` and `waitpid()`. The parent process created multiple child processes and synchronized their completion using the two functions. The Linux monitoring tools `ps`, `top`, and `/proc` were used to observe the process states during execution. The experiment showed that `waitpid()` offers finer control by waiting for a specific child process, while `wait()` waits for any remaining child process.