# Operating Systems Lab Report

# Experiment 2: File Copy Using `open()`, `read()`, `write()`, and `close()`

## Aim
To develop a C program that copies the contents of one file to another using the Linux system calls `open()`, `read()`, `write()`, and `close()`. Also, to understand how control transfers between user space and kernel space during file operations.

---

# Objectives
- Open an existing source file using `open()`.
- Create or open a destination file using `open()`.
- Read the contents of the source file using `read()`.
- Write the contents into the destination file using `write()`.
- Close both files using `close()`.
- Understand the transition between user space and kernel space during execution.

---

# Program Description
The program accepts the names of a source file and a destination file from the user.

- The source file is opened in **read-only mode**.
- The destination file is created (if it does not exist) and opened in **write mode**.
- The program repeatedly reads data from the source file into a buffer using the `read()` system call.
- The data is then written to the destination file using the `write()` system call.
- This process continues until the end of the source file is reached.
- Finally, both files are closed using the `close()` system call.

---

# Algorithm
1. Start the program.
2. Read the source file name from the user.
3. Read the destination file name from the user.
4. Open the source file using `open()`.
5. Create/open the destination file using `open()`.
6. Read data from the source file using `read()`.
7. Write the data into the destination file using `write()`.
8. Repeat Steps 6 and 7 until the end of the file is reached.
9. Close both files using `close()`.
10. Display a success message.
11. End the program.

---

# Sample Execution
```text
Enter source file: sample.txt
Enter destination file: cremate.txt

File copied successfully.
```

---

# Observation
The contents of `sample.txt` were successfully copied into `cremate.txt`. The destination file contained exactly the same data as the source file.

---

# System Calls Used

## 1. `open()`

### Purpose
Opens an existing file or creates a new file and returns a file descriptor that is used for subsequent file operations.

### Syntax
```c
int open(const char *pathname, int flags, mode_t mode);
```

---

## 2. `read()`

### Purpose
Reads data from the source file into a memory buffer.

### Syntax
```c
ssize_t read(int fd, void *buffer, size_t count);
```

---

## 3. `write()`

### Purpose
Writes data stored in the buffer into the destination file.

### Syntax
```c
ssize_t write(int fd, const void *buffer, size_t count);
```

---

## 4. `close()`

### Purpose
Closes an open file descriptor and releases the associated kernel resources.

### Syntax
```c
int close(int fd);
```

---

# User Space and Kernel Space Transition
Linux separates program execution into **User Space** and **Kernel Space**.

### User Space
- Where application programs execute.
- User programs cannot directly access hardware devices.
- File operations requested by the application are performed through system calls.

### Kernel Space
- Contains the Linux kernel.
- Manages hardware resources such as storage devices, memory, and I/O.
- Executes privileged operations requested through system calls.

---

## Execution Flow
```text
Application (User Space)
        │
        │ open()
        ▼
Linux Kernel (Kernel Space)
        │
        ▼
File Descriptor Returned
        │
        │ read()
        ▼
Kernel Reads Data from Disk
        │
        ▼
Buffer in User Space
        │
        │ write()
        ▼
Kernel Writes Data to Destination File
        │
        ▼
close()
        │
        ▼
Kernel Releases File Resources
```

---

# Relationship Between User Space and Kernel Space
| User Space | Kernel Space |
|------------|--------------|
| Runs application programs | Runs the Linux kernel |
| Cannot directly access hardware | Has complete access to hardware |
| Requests services using system calls | Performs requested operations |
| Receives results from the kernel | Returns results to the application |

---

# Advantages of System Calls
- Provide secure access to hardware resources.
- Prevent unauthorized hardware access.
- Ensure efficient file management.
- Allow multiple processes to safely share system resources.
- Improve system reliability and protection.

---

# Result
The C program successfully copied the contents of the source file to the destination file using the Linux system calls `open()`, `read()`, `write()`, and `close()`. The experiment also demonstrated how application programs running in user space interact with the Linux kernel through system calls to perform file operations securely and efficiently.