#include <stdio.h>
#include <unistd.h>
int main()
{
 printf("Before execlp()\n");
 execlp("date", "date", NULL);
 printf("This will not print if execlp() succeeds.\n");
 return 0;
}
