#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/*
 * SHELL ARCHITECTURE DOCUMENTATION
 * =================================
 * Flow:
 *   START -> Display Prompt -> Read Input -> Parse ->
 *   Execute (fg/bg/pipe/redirect) -> Wait/Monitor -> Repeat
 *   "exit" -> Cleanup -> END
 *
 * Key Syscalls:
 *   fork()    - create child process
 *   execvp()  - replace child with command
 *   waitpid() - wait for child
 *   pipe()    - connect processes
 *   dup2()    - redirect file descriptors
 *   kill()    - send signals
 *   setpgid() - set process group
 */

void show_arch() {
    printf("=== Shell Architecture ===\n\n");
    printf("START\n");
    printf("  |\n");
    printf("  v\n");
    printf("Display Prompt (>)\n");
    printf("  |\n");
    printf("  v\n");
    printf("Read User Input\n");
    printf("  |\n");
    printf("  v\n");
    printf("Is input == exit?\n");
    printf("  |YES          |NO\n");
    printf("  v             v\n");
    printf("Cleanup     Parse Input\n");
    printf("  |             |\n");
    printf("  v             v\n");
    printf("END         fork+exec\n");
    printf("              |\n");
    printf("              v\n");
    printf("         waitpid/bg\n");
    printf("              |\n");
    printf("              v\n");
    printf("         Display Prompt\n");
}

void show_syscalls() {
    printf("\n=== Key Syscalls ===\n");
    printf("%-12s %s\n", "fork()",    "create child process");
    printf("%-12s %s\n", "execvp()",  "replace child with cmd");
    printf("%-12s %s\n", "waitpid()", "wait for child exit");
    printf("%-12s %s\n", "pipe()",    "connect stdout->stdin");
    printf("%-12s %s\n", "dup2()",    "redirect file descriptors");
    printf("%-12s %s\n", "kill()",    "send signals to process");
    printf("%-12s %s\n", "setpgid()", "assign process group");
}

void show_design() {
    printf("\n=== Design Decisions ===\n");
    printf("1. Resource ordering prevents deadlock\n");
    printf("2. SIGINT forwarded to fg job, not shell\n");
    printf("3. Each child gets own process group\n");
    printf("4. Job table tracks state transitions\n");
    printf("5. All fds closed after dup2 to avoid leaks\n");
}

int main() {
    show_arch();
    show_syscalls();
    show_design();
    printf("\nDocumentation complete.\n");
    return 0;
}
