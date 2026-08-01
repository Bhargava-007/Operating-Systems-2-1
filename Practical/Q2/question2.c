#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(){
    char source[100], destination[100];
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    printf("Enter source file: ");
    scanf("%99s", source);

    printf("Enter destination file: ");
    scanf("%99s", destination);

    // Open source file (Read Only)
    int src = open(source, O_RDONLY);

    if (src < 0){
        perror("Error opening source file");
        return 1;
    }

    // Open/Create destination file
    int dest = open(destination,
                    O_WRONLY | O_CREAT | O_TRUNC,
                    0644);

    if (dest < 0){
        perror("Error opening destination file");
        close(src);
        return 1;
    }

    // Read from source and write to destination
    while ((bytesRead = read(src, buffer, BUFFER_SIZE)) > 0){
        write(dest, buffer, bytesRead);
    }

    if (bytesRead < 0){
        perror("Error reading source file");
    }

    close(src);
    close(dest);

    printf("\nFile copied successfully.\n");

    return 0;
}