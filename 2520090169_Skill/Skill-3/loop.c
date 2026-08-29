#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

void run_input_loop() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    History h = {0};
    int buf_size;
    char *buf = alloc_buffer(&buf_size);
    int len = 0;

    printf("=== Input Loop (Up/Down=history, Enter=store, Ctrl+C=quit) ===\n> ");
    fflush(stdout);

    int c;
    while ((c = getchar()) != 3) {   /* Ctrl+C = ASCII 3 */
        if (c == '\033') {
            getchar();               /* skip '[' */
            char seq = getchar();
            /* Apply escape sequences */
            if (seq == 'A') {        /* Up arrow - navigate previous */
                const char *p = navigate_prev(&h);
                printf("\r> %-*s\r> %s", len, "", p);
                len = strlen(p);
                strncpy(buf, p, buf_size - 1);
            } else if (seq == 'B') { /* Down arrow - navigate next */
                const char *n = navigate_next(&h);
                printf("\r> %-*s\r> %s", len, "", n);
                len = strlen(n);
                strncpy(buf, n, buf_size - 1);
            }
        } else if (c == 127 || c == '\b') {  /* Backspace */
            if (len > 0) {
                len--;
                buf[len] = '\0';
                printf("\b \b");
            }
        } else if (c == '\n') {      /* Enter - update input buffer & store */
            buf[len] = '\0';
            printf("\n");
            if (len > 0) {
                store_command(&h, buf);
                printf("[Stored #%d]: %s\n> ", h.count, buf);
            } else {
                printf("> ");
            }
            len = 0;
            buf[0] = '\0';
        } else {
            /* Resize array if needed - prevent buffer overflow */
            if (len >= buf_size - 1)
                buf = resize_buffer(buf, &buf_size);
            buf[len++] = c;
            buf[len] = '\0';
            putchar(c);
        }
        fflush(stdout);
    }

    printf("\n[Exiting] History had %d command(s).\n", h.count);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    /* Release memory correctly */
    free_history(&h);
    free(buf);
}
