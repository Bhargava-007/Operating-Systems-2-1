#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int read_input(char *buffer, int size)
{
    struct termios original;
    struct termios raw;
    int position = 0;
    char character;

    tcgetattr(STDIN_FILENO, &original);

    raw = original;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    while (1) {
        read(STDIN_FILENO, &character, 1);

        if (character == '\n' || character == '\r') {
            buffer[position] = '\0';
            putchar('\n');
            break;
        }

        if (character == 127 || character == '\b') {
            if (position > 0) {
                position--;
                printf("\b \b");
                fflush(stdout);
            }

            continue;
        }

        if (position < size - 1) {
            buffer[position++] = character;
            putchar(character);
            fflush(stdout);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &original);

    return position;
}