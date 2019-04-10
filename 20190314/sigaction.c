#include<stdio.h>
#include<signal.h>
#include<string.h>

void sigFunc(int signum,siginfo_t *p,void *p1)
{
    printf("%d号来啦\n",signum);
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sigFunc;
    act.sa_flags = SA_SIGINFO;
    int ret;
    ret=sigaction(SIGINT, &act, NULL);
    if(-1==ret)
    {
        perror("sigaction");
    }
    while(1);
}
