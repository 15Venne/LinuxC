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
    stat=time(NULL);
    printf("%s\n", ctime(&stat));
}

int main()
{
    signal(SIGPROF,sigFunc);
    sigFunc(0);
    struct itimerval tm;
    memset(&tm, 0, sizeof(tm));
    tm.it_value.tv_sec = 3;
    tm.it_interval.tv_sec = 2;
    setitimer(ITIMER_PROF, &tm, NULL);
    sleep(4);
    while(1);
}
