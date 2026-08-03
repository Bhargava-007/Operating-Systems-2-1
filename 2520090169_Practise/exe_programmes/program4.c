#include <stdio.h>
#include <unistd.h>
int main()
{
char *args[] = {"date"
, NULL};
printf("Before execvp()\n");
execvp("date"
, args);
printf("This will not print if execvp() succeeds.\n");
return 0;
}
