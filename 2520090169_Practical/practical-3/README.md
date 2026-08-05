# Q3 - Process Creation using `fork()`

## Aim

To develop a C program using `fork()` that creates a parent and child process, displays the Process ID (PID) and Parent Process ID (PPID), and observes process state transitions using Linux monitoring tools (`ps`, `top`, and `/proc`).

---

## Requirements

- GCC Compiler
- Linux Operating System (Ubuntu/Kali/WSL)
- Terminal

---

## Files

```
Q3/
├── fork_process.c
├── README.md
└── screenshots/
    ├── 01_compile.png
    ├── 02_program_output.png
    ├── 03_top_process_state.png
    ├── 04_proc_status.png
    └── 05_process_terminated.png
```

---

## Algorithm

1. Start the program.
2. Create a child process using `fork()`.
3. Display the Parent PID and Child PID.
4. Put the child process to sleep using `sleep()` to observe its state.
5. Make the parent wait for the child using `wait()`.
6. Observe the process state using:
   - `ps`
   - `top`
   - `/proc/<PID>/status`
7. After the child terminates, the parent also exits.

---

## Source Code

See `fork_process.c`.

---

## Compilation

```bash
gcc fork_process.c -o fork_process
```

---

## Execution

```bash
./fork_process
```

---

## Sample Output

```
----- PARENT PROCESS -----
Parent PID: 5444
Parent PPID: 639
Child PID : 5445
Parent waiting for child...

----- CHILD PROCESS -----
Child PID : 5445
Parent PID: 5444
Child is sleeping for 30 seconds...

Child process exiting.
Child terminated.
Parent exiting.
```

---

## Commands Used During Observation

### View process using `ps`

```bash
ps -o pid,ppid,state,cmd | grep fork_process
```

### Monitor process using `top`

```bash
top
```

### View process information

```bash
cat /proc/<child_pid>/status
```

---

## Process State Observations

| State | Observation |
|-------|-------------|
| Ready | The process briefly enters the ready queue before CPU scheduling. This state is too short to observe directly using `ps` or `top`. |
| Running | The parent and child processes execute immediately after `fork()`. |
| Waiting | The child enters the sleeping state during `sleep()`, displayed as `S` in `top` and `/proc/<PID>/status`. The parent waits for the child using `wait()`. |
| Terminated | After execution, both parent and child processes terminate and no longer appear in the process table. |

---

## Linux Monitoring Tools Used

- `ps`
- `top`
- `/proc/<PID>/status`

---

## Conclusion

The experiment successfully demonstrated process creation using `fork()`. The parent and child processes were created with unique Process IDs, while the Parent Process ID correctly identified the parent process. The Linux monitoring tools `ps`, `top`, and `/proc` were used to observe process execution and state transitions. The child process entered the waiting (sleeping) state during `sleep()`, and the parent synchronized its execution using `wait()`. Both processes terminated successfully after completing execution.

---

## Screenshots

- program_output.png
- ps_process_state.png
- top_process_state.png
- ps_process_state.png
- process_terminated.png