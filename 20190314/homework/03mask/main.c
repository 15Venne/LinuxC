#include<stdio.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
//sigaction实现2号信号，屏蔽3号信号的执行效果，
//同时在2号信号的信号处理流程中，判断3号信号是否被挂起
void sigFunc(int signum, siginfo_t *p, void *q)
{
    printf("------------------------------\n");
    printf(" %d号来啦\n\n", signum);
    sleep(5);
    sigset_t pend;
    sigpending(&pend);
    if(sigismember(&pend,SIGQUIT))
    {
        printf("但是！3号也来了\n\n");
    }
    else
    {
        printf("3号没有来过\n\n");
    }
    printf(" %d醒啦\n\n",signum);
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sigFunc;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);//清空集合
    sigaddset(&act.sa_mask, SIGQUIT);//3号填入信号集
    int ret;
    ret = sigaction(SIGINT, &act, NULL);//重新设定2号行为
    if(-1 == ret)
    {
        perror("sigaction");
    }
    ret = sigaction(SIGQUIT, &act, NULL);//重新设定3号行为
    while(1);
}
