#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define OUTPUT_FILE "output.txt"
#define INPUT_FILE  "input.txt"

void demo_stdout_redirect() {
    printf("\n========================================\n");
    printf("  Demo 1: Redirect stdout to file\n");
    printf("========================================\n");

    int fd = open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); exit(1); }

    pid_t pid = fork();
    if (pid == 0) {
        /* Child: redirect stdout (fd=1) to file using dup2 */
        dup2(fd, STDOUT_FILENO);
        close(fd);
        printf("This line goes to file, not terminal!\n");
        printf("Shell does this for: command > output.txt\n");
        printf("PID %d writing to redirected stdout\n", getpid());
        exit(0);
    }
    close(fd);
    wait(NULL);
    printf("[Parent] Child wrote to '%s' via dup2()\n", OUTPUT_FILE);

    /* Read back and show */
    printf("[Parent] Reading back '%s':\n", OUTPUT_FILE);
    fd = open(OUTPUT_FILE, O_RDONLY);
    char buf[256];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("  > %s", buf);
    }
    close(fd);
}

void demo_stdin_redirect() {
    printf("\n========================================\n");
    printf("  Demo 2: Redirect stdin from file\n");
    printf("========================================\n");

    /* Create input file */
    int fd = open(INPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open input"); exit(1); }
    const char *data = "Hello from input file!\nSecond line of input!\n";
    write(fd, data, strlen(data));
    close(fd);

    fd = open(INPUT_FILE, O_RDONLY);
    if (fd < 0) { perror("open input read"); exit(1); }

    pid_t pid = fork();
    if (pid == 0) {
        /* Child: redirect stdin (fd=0) from file using dup2 */
        dup2(fd, STDIN_FILENO);
        close(fd);
        printf("[Child] Reading from redirected stdin:\n");
        char line[128];
        while (fgets(line, sizeof(line), stdin))
            printf("  [stdin] -> %s", line);
        exit(0);
    }
    close(fd);
    wait(NULL);
}

void demo_pipe_redirect() {
    printf("\n========================================\n");
    printf("  Demo 3: Pipe redirection (cmd1 | cmd2)\n");
    printf("========================================\n");

    int pipefd[2];
    if (pipe(pipefd) < 0) { perror("pipe"); exit(1); }

    pid_t pid = fork();
    if (pid == 0) {
        /* Child (writer): redirect stdout to pipe write end */
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        printf("Data from child through pipe!\n");
        printf("This simulates: echo 'hello' | cat\n");
        exit(0);
    }

    /* Parent (reader): read from pipe read end */
    close(pipefd[1]);
    wait(NULL);
    char buf[256];
    ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
    buf[n] = '\0';
    close(pipefd[0]);
    printf("[Parent] Received from pipe:\n  > %s", buf);
}

int main() {
    printf("========================================\n");
    printf("   I/O Redirection using dup2()\n");
    printf("========================================\n");
    printf("File Descriptors: stdin=0  stdout=1  stderr=2\n");

    demo_stdout_redirect();
    demo_stdin_redirect();
    demo_pipe_redirect();

    printf("\n========================================\n");
    printf("  Summary\n");
    printf("========================================\n");
    printf("dup2(fd, STDOUT_FILENO) -> cmd > file\n");
    printf("dup2(fd, STDIN_FILENO)  -> cmd < file\n");
    printf("dup2(pipe[1], STDOUT)   -> cmd1 | cmd2\n");
    printf("Shells use exactly this mechanism!\n");

    return 0;
}
