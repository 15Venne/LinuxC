#include<stdio.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
//通过sigprocmask阻塞2号信号，睡眠5秒后，解除阻塞，2号信号得到执行；
//在睡眠后，解除阻塞之前，通过sigpending检测是否有信号挂起
int main()
{
    sigset_t procmask;
    sigemptyset(&procmask);
    sigaddset(&procmask, SIGINT);//2号加入
    sigprocmask(SIG_BLOCK, &procmask, NULL);//把参数二加入阻塞集合
    printf("\n\n----------KEEP---------OUT----------\n");
    printf("-                                  -\n");
    printf("----------KEEP---------OUT----------\n");
    sleep(5);
    //检测是否有信号挂起
    sigset_t pend;
    sigpending(&pend);
    if(sigismember(&pend, SIGINT))
    {
        printf("\n2号来过\n");
    }
    else
    {
        printf("\n2号没有来过\n");
    }
    sigprocmask(SIG_UNBLOCK, &procmask, NULL); //解除阻塞
}
