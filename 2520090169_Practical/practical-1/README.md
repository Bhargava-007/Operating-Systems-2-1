# Operating Systems Lab Report

## Experiment 1: Process Creation and Command Execution using `fork()`, `exec()`, and `wait()`

### Aim
To develop a C program that demonstrates how the Linux operating system executes a command entered by a user using the `fork()`, `exec()`, and `wait()` system calls.

---

## Objectives
- Accept a Linux command as input from the user.
- Create a child process using `fork()`.
- Execute the command in the child process using `execlp()`.
- Allow the parent process to wait for the child using `wait()`.
- Display the Process IDs (PIDs) of both the parent and child processes.

---

## Program Description
The program first prompts the user to enter a Linux command. It then creates a child process using the `fork()` system call.

- The **parent process** displays its Process ID (PID), the child PID, and waits for the child process to complete using `wait()`.
- The **child process** displays its own PID and its parent PID, then executes the user-entered Linux command using `execlp()`.
- After the child process finishes execution, the parent process resumes and prints a completion message.

This demonstrates process creation, execution, synchronization, and process identification in Linux.

---

## Algorithm
1. Start the program.
2. Read a Linux command from the user.
3. Create a child process using `fork()`.
4. If `fork()` fails, display an error message and terminate.
5. If the process is the child:
   - Display the child PID and parent PID.
   - Execute the entered command using `execlp()`.
6. If the process is the parent:
   - Display the parent PID and child PID.
   - Wait for the child process to complete using `wait()`.
7. Display a completion message.
8. End the program.

---

## Sample Outputs

### Example 1
```text
Enter a Linux command: uname

Parent Process
Parent PID : 2919
Child PID  : 2947

Child Process
Child PID : 2947
Parent PID: 2919

Linux

Child process completed.
```

### Example 2
```text
Enter a Linux command: ps

Parent Process
Parent PID : 3130
Child PID  : 3155

Child Process
Child PID : 3155
Parent PID: 3130

    PID TTY          TIME CMD
    760 pts/4    00:00:00 bash
   3130 pts/4    00:00:00 question1
   3155 pts/4    00:00:00 ps

Child process completed.
```

---

## Result
The program successfully demonstrated process creation using `fork()`, command execution using `execlp()`, process synchronization using `wait()`, and displayed the Process IDs (PIDs) of both the parent and child processes.

---

# Investigation of Linux Commands

## Aim
To investigate the relationship between hardware resources and operating system services using Linux terminal commands.

---

# 1. `uname`

## Purpose
Displays information about the Linux operating system kernel.

### Command
```bash
uname
```

### Output
```text
Linux
```

### Observation
The command displays the name of the operating system kernel.

### Operating System Service
The Linux kernel is responsible for managing communication between hardware resources and application software. It provides an abstraction layer that hides hardware complexity from users and applications.

---

# 2. `lscpu`

## Purpose
Displays detailed information about the processor.

### Command

```bash
lscpu
```

### Observation

The output from the system shows:

- Architecture: x86_64
- CPU Model: Intel(R) Core(TM) i5-1135G7
- CPU(s): 8
- Threads per Core: 2
- Cores per Socket: 4
- Virtualization Support: VT-x

### Operating System Service

The operating system abstracts the processor by scheduling CPU time among multiple processes. Applications do not communicate directly with the processor; instead, the operating system manages process scheduling and execution.

---

# 3. `lsblk`

## Purpose
Displays information about available storage devices.

### Command
```bash
lsblk
```

### Observation
The system contains the following storage devices:

| Device | Description |
|---------|-------------|
| sda | Disk |
| sdb | Disk |
| sdc | Swap Device |
| sdd | 1 TB Disk |

### Operating System Service
The operating system abstracts physical storage by providing a file system. Users interact with files and directories rather than directly accessing disk sectors.

---

# 4. `ps`

## Purpose
Displays information about currently running processes.

### Command
```bash
ps
```

### Observation
The output lists currently running processes such as:

- bash
- question1
- ps

Each process has a unique Process ID (PID).

### Operating System Service
The operating system creates, schedules, monitors, and terminates processes. It ensures that multiple programs can execute simultaneously without interfering with each other.

---

# 5. `top`

## Purpose
Displays real-time information about CPU usage, memory usage, and running processes.

### Command
```bash
top
```

### Observation
The system displayed:

- CPU Idle: 98.7%
- Total Memory: 3787.6 MiB
- Used Memory: 655.9 MiB
- Free Memory: 2985.9 MiB
- Swap Memory: 1024 MiB

### Operating System Service
The operating system dynamically allocates CPU time and memory to processes, monitors system performance, and efficiently manages hardware resources.

---

# Relationship Between Hardware Resources and Operating System Services
| Hardware Resource | Linux Command | Operating System Service |
|-------------------|---------------|--------------------------|
| CPU | `lscpu` | Schedules processor time among processes and manages execution. |
| Memory | `top` | Allocates and manages RAM for running processes while providing memory protection. |
| Storage | `lsblk` | Provides file system abstraction and manages storage devices and partitions. |
| Processes | `ps` | Creates, schedules, synchronizes, and terminates processes. |
| Operating System | `uname` | Provides kernel information and manages communication between software and hardware. |

---

# Conclusion
The Linux operating system provides an abstraction layer between application software and hardware resources. It efficiently manages the CPU through process scheduling, allocates memory to processes, organizes storage using a file system, and controls process execution using system calls such as `fork()`, `exec()`, and `wait()`.

The Linux commands `uname`, `lscpu`, `lsblk`, `ps`, and `top` demonstrate how the operating system monitors and manages hardware resources while providing essential services to users and applications. The experiment successfully illustrates the relationship between hardware resources and operating system services.