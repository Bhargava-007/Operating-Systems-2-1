#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#define FILE_SIZE (1024 * 1024)
#define FILENAME "testfile.bin"

long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

void create_test_file() {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char buf[FILE_SIZE];
    memset(buf, 'A', FILE_SIZE);
    write(fd, buf, FILE_SIZE);
    close(fd);
}

void mmap_read_write() {
    long start, end;

    // mmap write
    int fd = open(FILENAME, O_RDWR);
    start = get_time_ns();
    char *map = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memset(map, 'B', FILE_SIZE);
    msync(map, FILE_SIZE, MS_SYNC);
    munmap(map, FILE_SIZE);
    end = get_time_ns();
    printf("mmap  WRITE: %ld ns\n", end - start);
    close(fd);

    // mmap read
    fd = open(FILENAME, O_RDONLY);
    start = get_time_ns();
    map = mmap(NULL, FILE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    char tmp = 0;
    for (int i = 0; i < FILE_SIZE; i += 4096) tmp ^= map[i];
    munmap(map, FILE_SIZE);
    end = get_time_ns();
    printf("mmap  READ:  %ld ns  (checksum=%d)\n", end - start, tmp);
    close(fd);
}

void traditional_read_write() {
    long start, end;
    char buf[4096];

    // traditional write
    int fd = open(FILENAME, O_WRONLY);
    start = get_time_ns();
    memset(buf, 'C', sizeof(buf));
    for (int i = 0; i < FILE_SIZE / 4096; i++) write(fd, buf, 4096);
    end = get_time_ns();
    printf("read/write WRITE: %ld ns\n", end - start);
    close(fd);

    // traditional read
    fd = open(FILENAME, O_RDONLY);
    start = get_time_ns();
    char tmp = 0;
    while (read(fd, buf, 4096) > 0) tmp ^= buf[0];
    end = get_time_ns();
    printf("read/write READ:  %ld ns  (checksum=%d)\n", end - start, tmp);
    close(fd);
}

int main() {
    create_test_file();

    printf("=== mmap vs Traditional read/write ===\n");
    printf("File size: %d bytes (1MB)\n\n", FILE_SIZE);

    printf("--- mmap ---\n");
    mmap_read_write();

    printf("\n--- Traditional read/write ---\n");
    traditional_read_write();

    printf("\n=== Summary ===\n");
    printf("mmap:      Maps file into virtual memory. Zero-copy, OS handles paging.\n");
    printf("read/write: Explicit syscalls per chunk. More overhead, simpler control.\n");

    remove(FILENAME);
    return 0;
}
