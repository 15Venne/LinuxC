#include<stdio.h>
#include<signal.h>
#include<string.h>

//sigaction实现信号不断重入的执行效果

void sigFunc(int signum, siginfo_t *p, void *q)
{
    printf(" %d号来啦\n\n", signum);
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sigFunc;
    act.sa_flags = SA_SIGINFO;
    int ret=sigaction(SIGINT, &act, NULL);
    if(-1==ret)
    {
        perror("sigaction");
    }
    while(1);
}
