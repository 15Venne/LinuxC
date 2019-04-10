#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
    pid_t pid;
    pid=fork();
    if(0==pid)
    {
        printf("i am child.pid=%d\n",getpid());
        sleep(15);
        return 0;
    }
    else
    {
        printf("i am parent.pid=%d\n",getppid());
        return 0;
    }
}
