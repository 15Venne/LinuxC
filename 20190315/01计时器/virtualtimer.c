#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
#include<time.h>
#include<signal.h>
#include<string.h>
#include<string.h>
#include<string.h>


void sigFunc(int signum)
{
    time_t stat;
    time(&stat);
    printf("%s\n",ctime(&stat));
}

int main()
{
    signal(SIGVTALRM,sigFunc);
    sigFunc(0);
    struct itimerval tm;
    memset(&tm, 0, sizeof(tm));
    tm.it_value.tv_sec = 3;
    tm.it_interval.tv_sec = 2;
    setitimer(ITIMER_VIRTUAL, &tm, NULL);
    sleep(4);
    while(1);
}
