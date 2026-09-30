#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void print_inode_info(const char *path) {
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("stat");
        return;
    }
    printf("\n--- %s ---\n", path);
    printf("Inode:      %lu\n", st.st_ino);
    printf("Hard links: %lu\n", st.st_nlink);
    printf("Size:       %ld bytes\n", st.st_size);
    printf("UID:        %u\n", st.st_uid);
    printf("Permissions:%o\n", st.st_mode & 0777);
}

int main() {
    // Create original file
    FILE *f = fopen("original.txt", "w");
    fprintf(f, "Inode investigation file\n");
    fclose(f);

    printf("=== Original File ===");
    print_inode_info("original.txt");

    // Hard link
    link("original.txt", "hardlink.txt");
    printf("\n=== After Hard Link ===");
    print_inode_info("original.txt");
    print_inode_info("hardlink.txt");

    // Symbolic link
    symlink("original.txt", "symlink.txt");
    printf("\n=== Symbolic Link Info ===");
    print_inode_info("symlink.txt");

    struct stat lst;
    lstat("symlink.txt", &lst);
    printf("\n--- symlink.txt (lstat - link itself) ---\n");
    printf("Inode:      %lu\n", lst.st_ino);
    printf("Hard links: %lu\n", lst.st_nlink);
    printf("Size:       %ld bytes\n", lst.st_size);

    printf("\n=== Shell Commands Output ===\n");
    system("ls -i original.txt hardlink.txt symlink.txt");
    printf("\n");
    system("stat original.txt");

    // Cleanup
    remove("original.txt");
    remove("hardlink.txt");
    remove("symlink.txt");

    return 0;
}
