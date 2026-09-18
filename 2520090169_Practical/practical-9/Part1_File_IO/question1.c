#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 4096
#define TEST_FILE   "test_input.txt"
#define COPY1       "copy_syscall.txt"
#define COPY2       "copy_stdio.txt"

/* Create a test file with sample data */
void create_test_file() {
    int fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open test file"); exit(1); }
    for (int i = 0; i < 1000; i++) {
        char line[64];
        snprintf(line, sizeof(line), "Line %04d: Hello from OS Practical-9!\n", i + 1);
        write(fd, line, strlen(line));
    }
    close(fd);
    printf("Test file '%s' created.\n", TEST_FILE);
}

/* File copy using low-level system calls */
double copy_syscall(const char *src, const char *dst) {
    char buf[BUFFER_SIZE];
    int in_fd  = open(src, O_RDONLY);
    if (in_fd < 0) { perror("open src"); exit(1); }
    int out_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) { perror("open dst"); exit(1); }

    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    ssize_t bytes;
    while ((bytes = read(in_fd, buf, BUFFER_SIZE)) > 0)
        write(out_fd, buf, bytes);

    clock_gettime(CLOCK_MONOTONIC, &t2);

    /* lseek demo */
    off_t size = lseek(in_fd, 0, SEEK_END);
    printf("[syscall] File size via lseek: %ld bytes\n", size);

    close(in_fd);
    close(out_fd);

    double elapsed = (t2.tv_sec - t1.tv_sec) * 1e9 + (t2.tv_nsec - t1.tv_nsec);
    return elapsed;
}

/* File copy using standard library functions */
double copy_stdio(const char *src, const char *dst) {
    char buf[BUFFER_SIZE];
    FILE *in  = fopen(src, "r");
    if (!in)  { perror("fopen src"); exit(1); }
    FILE *out = fopen(dst, "w");
    if (!out) { perror("fopen dst"); exit(1); }

    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    size_t bytes;
    while ((bytes = fread(buf, 1, BUFFER_SIZE, in)) > 0)
        fwrite(buf, 1, bytes, out);

    clock_gettime(CLOCK_MONOTONIC, &t2);

    fclose(in);
    fclose(out);

    double elapsed = (t2.tv_sec - t1.tv_sec) * 1e9 + (t2.tv_nsec - t1.tv_nsec);
    return elapsed;
}

int main() {
    printf("========================================\n");
    printf("   File I/O: syscall vs stdio\n");
    printf("========================================\n\n");

    create_test_file();

    printf("\n--- Copying using system calls (open/read/write/lseek/close) ---\n");
    double t1 = copy_syscall(TEST_FILE, COPY1);
    printf("[syscall] Copy done -> '%s'  Time: %.2f ns\n", COPY1, t1);

    printf("\n--- Copying using stdio (fopen/fread/fwrite/fclose) ---\n");
    double t2 = copy_stdio(TEST_FILE, COPY2);
    printf("[stdio]   Copy done -> '%s'   Time: %.2f ns\n", COPY2, t2);

    printf("\n========================================\n");
    printf("  Performance Comparison\n");
    printf("========================================\n");
    printf("syscall time : %.2f ns\n", t1);
    printf("stdio   time : %.2f ns\n", t2);
    if (t1 < t2)
        printf("Result: syscall was FASTER by %.2f ns\n", t2 - t1);
    else
        printf("Result: stdio was FASTER by %.2f ns\n", t1 - t2);

    printf("\n--- Verifying copies with lseek ---\n");
    int fd1 = open(COPY1, O_RDONLY);
    int fd2 = open(COPY2, O_RDONLY);
    printf("Size of '%s' : %ld bytes\n", COPY1, lseek(fd1, 0, SEEK_END));
    printf("Size of '%s'  : %ld bytes\n", COPY2, lseek(fd2, 0, SEEK_END));
    close(fd1);
    close(fd2);

    return 0;
}
