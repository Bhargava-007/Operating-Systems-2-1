#include <stdio.h>
#include <unistd.h>
extern char **environ;
int main()
{
char *args[] = {"ls"
,
"-l"
, NULL};
printf("Before execve()\n");
execve("/bin/ls"
, args, environ);
printf("This will not print if execve() succeeds.\n");
return 0;
}
