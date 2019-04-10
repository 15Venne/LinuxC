#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<string.h>
int main()
{
    time_t st;
    time_t temp;
    int i;
    time(&st);
    printf("st=%ld\n",st);
    struct tm *p=gmtime(&st);
    printf("%d年%d月%d日%d时%02d分%d秒\n",1900+p->tm_year, p->tm_mon, p->tm_mday, 8+p->tm_hour, p->tm_min, p->tm_sec);
    struct tm q;
    q.tm_year = p->tm_year;
    q.tm_mon = p->tm_mon;
    q.tm_mday = p->tm_mday;
    q.tm_hour = p->tm_hour+8;  //?
    q.tm_min = p->tm_min;
    q.tm_sec = p->tm_sec;
    temp=mktime(&q);
    printf("%ld\n",temp);
    struct timeval tv1,tv2;
    gettimeofday(&tv1,NULL);
    for(i=0;i<100000000;++i)
    {
    
    }
    gettimeofday(&tv2,NULL);
    printf("i=%d\n",i);
    printf("经过了%ld\n",(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
    return 0;
    
}
