#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
#include<time.h>
#include<signal.h>
#include<string.h>
#include<string.h>
#include<string.h>

time_t begin;

void sigFunc(int signum)
{
    time_t stat;
    time(&stat);
    printf("%lds\n", stat - begin);
}

int main()
{
    time(&begin);
    signal(SIGALRM,sigFunc);
    struct itimerval tm;
    memset(&tm, 0, sizeof(tm));
    tm.it_value.tv_sec = 3;
    tm.it_interval.tv_sec = 2;
    setitimer(ITIMER_REAL, &tm, NULL);
    while(1);
}
