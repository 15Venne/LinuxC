#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
int main()
{
    pid_t pid;
    pid=fork();
    int status;
    if(0==pid)
    {
        printf("i am child.pid=%d ppid=%d\n",getpid(),getppid());
        sleep(15);
        return 15;
    }
    else 
    {
        printf("i am parent.pid=%d\n",getpid());
       pid=wait(&status);
       if(WIFEXITED(status))
       {
           printf("i am parent.pid=%d\n",getpid());
           printf("child's pid=%d,return=%d\n",pid,WEXITSTATUS(status));
           return 0;
       }
    }
}
