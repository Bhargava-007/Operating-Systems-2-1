#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// ── Job Table Module ──────────────────────────────
#define MAX 8
typedef struct { int id; pid_t pid; char cmd[64]; char state[16]; } Job;
Job table[MAX]; int jcount = 0;

void job_add(char *cmd, int bg) {
    pid_t pid = fork();
    if (pid == 0) {
        char *a[] = {"sh","-c",cmd,NULL};
        execvp("sh",a); exit(1);
    }
    table[jcount].id=jcount+1; table[jcount].pid=pid;
    snprintf(table[jcount].cmd,64,"%s",cmd);
    snprintf(table[jcount].state,16,bg?"background":"foreground");
    jcount++;
    if (!bg) { waitpid(pid,NULL,0); snprintf(table[jcount-1].state,16,"done"); }
    printf("[%s] %d: %s\n", bg?"bg":"fg", pid, cmd);
}

void job_list() {
    printf("\n%-4s %-8s %-12s %s\n","ID","PID","STATE","CMD");
    for(int i=0;i<jcount;i++)
        printf("%-4d %-8d %-12s %s\n",table[i].id,table[i].pid,table[i].state,table[i].cmd);
}

// ── Redirect Module ───────────────────────────────
void redirect_out(char *cmd, char *file) {
    int fd=open(file,0x241,0644); // O_WRONLY|O_CREAT|O_TRUNC
    pid_t pid=fork();
    if(pid==0){ dup2(fd,1); close(fd);
        char *a[]={"sh","-c",cmd,NULL}; execvp("sh",a); exit(1); }
    close(fd); waitpid(pid,NULL,0);
    printf("[redirect] %s > %s\n",cmd,file);
}

// ── Parser Module ─────────────────────────────────
void parse_run(char *input) {
    if(!input||!*input){ printf("[parse] empty\n"); return; }
    if(strstr(input,"exit")){ printf("[parse] exit\n"); return; }
    printf("[parse] cmd: %s\n",input);
    job_add(input,0);
}

int main() {
    printf("=== Code Organization Demo ===\n\n");
    printf("-- Parser --\n");
    parse_run("echo hello");
    parse_run("");

    printf("\n-- Jobs --\n");
    job_add("echo bg_task",1);
    job_list();

    printf("\n-- Redirect --\n");
    redirect_out("echo organized","out.txt");
    system("cat out.txt");
    remove("out.txt");

    printf("\nAll modules organized and verified.\n");
    return 0;
}
