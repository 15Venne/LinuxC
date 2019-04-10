#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
    pid_t pid;
    pid=fork();
    if(0==pid)
    {
        printf("i am child.\n");
    }
    else
    {
        printf("i am parent.\n");
        while(1);
    }
    return 0;
}
