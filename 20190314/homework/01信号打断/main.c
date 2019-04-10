#include<signal.h>
#include<stdio.h>
#include<unistd.h>
//signal设置信号处理行为，演示不同信号到来时，
//后来的信号会打断原有信号的信号处理函数效果
void sigFunc(int signum)
{
    printf(" %d号来啦！\n\n",signum);
    sleep(3);
}

typedef void (*sigFun_t)(int);

int main()
{
    sigFun_t ret = signal(SIGINT, sigFunc);
    if(ret==SIG_ERR)
    {
        perror("signal");
    }
    signal(SIGQUIT, sigFunc);
    while(1);
}
