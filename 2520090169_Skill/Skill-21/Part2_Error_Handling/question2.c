#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

void log_error(char *context, char *msg) {
    fprintf(stderr, "[ERROR] %s: %s\n", context, msg);
}

void run_cmd(char *cmd) {
    if (!cmd || strlen(cmd) == 0) {
        log_error("parse", "empty command");
        return;
    }
    pid_t pid = fork();
    if (pid < 0) { log_error("fork", strerror(errno)); return; }
    if (pid == 0) {
        char *args[] = {"sh", "-c", cmd, NULL};
        execvp("sh", args);
        log_error("exec", strerror(errno));
        exit(1);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        log_error("exit", "non-zero exit status");
    else
        printf("[OK] %s\n", cmd);
}

void open_file(char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) { log_error("open", strerror(errno)); return; }
    printf("[OK] opened %s\n", path);
    close(fd);
}

int main() {
    printf("=== Error Handling ===\n\n");

    printf("Test 1: valid command\n");
    run_cmd("echo hello");

    printf("\nTest 2: empty command\n");
    run_cmd("");

    printf("\nTest 3: invalid command\n");
    run_cmd("notacommand_xyz");

    printf("\nTest 4: valid file\n");
    open_file("/etc/hostname");

    printf("\nTest 5: missing file\n");
    open_file("/no/such/file.txt");

    printf("\nAll error cases handled gracefully.\n");
    return 0;
}
