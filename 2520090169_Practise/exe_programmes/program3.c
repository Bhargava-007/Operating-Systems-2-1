#include <stdio.h>
#include <unistd.h>
int main()
{
char *args[] = {"ls"
,
"-l"
, NULL};
printf("Before execv()\n");
execv("/bin/ls"
, args);
printf("This will not print if execv() succeeds.\n");
return 0;
}
