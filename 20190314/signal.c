#include<stdio.h>
#include<signal.h>
#include<unistd.h>

typedef void (*sigFun_t)(int);

void sigFunc(int signum)
{
    printf("%d号来啦\n",signum);
}

int main()
{
    sigFun_t ret;
    ret=signal(SIGINT,sigFunc);
    if(ret==SIG_ERR)
    {
        perror("signal");
    }
    signal(SIGQUIT,sigFunc);
    char buf[128]={0};
    int retVal;
    retVal=read(0,buf,sizeof(buf));
    printf("retVal=%d,buf=%s\n",retVal,buf);
}
