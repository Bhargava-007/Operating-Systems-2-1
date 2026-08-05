# Q4 - Zombie Process and Elimination

## Aim

To create a zombie process, observe it using Linux process monitoring tools, and eliminate it using proper process synchronization with `wait()`.

---

## Requirements

- Linux Operating System
- GCC Compiler
- Terminal

---

## Files

```text
Part2_Zombie_Process/
├── zombie_process.c
├── zombie_fixed.c
├── README.md
└── screenshots/
    ├── 01_zombie_output.png
    ├── 02_zombie_process_table.png
    ├── 03_zombie_fixed_output.png
    └── 04_zombie_removed.png
```

---

## Algorithm

### Part A – Creating a Zombie Process

1. Start the program.
2. Create a child process using `fork()`.
3. The child process exits immediately.
4. The parent process does not call `wait()` and sleeps for some time.
5. During this period, the child becomes a zombie process.
6. Observe the zombie process using `ps`.

### Part B – Eliminating the Zombie Process

1. Start the modified program.
2. Create a child process using `fork()`.
3. The child exits normally.
4. The parent immediately calls `wait()`.
5. The child process is collected by the parent.
6. Verify that no zombie process exists.

---

## Source Code

- `zombie_process.c`
- `zombie_fixed.c`

---

## Compilation

```bash
gcc zombie_process.c -o zombie_process
gcc zombie_fixed.c -o zombie_fixed
```

---

## Execution

```bash
./zombie_process
```

```bash
./zombie_fixed
```

---

## Commands Used

### Display Process Table

```bash
ps -el | grep zombie_process
```

```bash
ps -el | grep zombie_fixed
```

---

## Observations

- The child process exited immediately while the parent continued executing.
- Since the parent did not call `wait()`, the child entered the **Zombie (Z)** state.
- The `ps` command displayed the child process with state **Z**, confirming the zombie process.
- In the modified program, the parent called `wait()` immediately after creating the child.
- The child process was successfully collected, preventing the creation of a zombie process.
- The final process table showed that no zombie process remained.

---

## Result

The experiment successfully demonstrated the creation of a zombie process when the parent did not synchronize with the child process. By using `wait()`, the parent collected the terminated child process, eliminating the zombie state and ensuring proper process synchronization.

---

## Conclusion

Zombie processes occur when a child process terminates but its parent does not collect its exit status. Proper synchronization using `wait()` or `waitpid()` allows the operating system to release the child process resources, preventing zombie processes and maintaining an accurate process table.