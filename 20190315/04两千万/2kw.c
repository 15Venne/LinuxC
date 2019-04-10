#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<sys/time.h>

#define N 20000000

typedef struct data
{
    int num;
    pthread_mutex_t mutex;
}data;


data n;

void *threadFunc(void *p)
{
    int i;
    //data *pc=(data*)p;
    for(i=0;i<N;++i)
    {
        pthread_mutex_lock(&n.mutex);
        n.num=n.num + 1;
        pthread_mutex_unlock(&n.mutex);
    }
    pthread_exit(0);
}

int main()
{
    n.num=0;
    pthread_t pthread;
    int ret, i, j=1000000;
    pthread_mutex_init(&n.mutex,NULL);
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    ret=pthread_create(&pthread, NULL, threadFunc, NULL);
    if(ret!=0)
    {
        printf("pthread_create failed %d\n", ret);
        return -1;
    }
    for(i=0;i<N;++i)
    {
        pthread_mutex_lock(&n.mutex);
        n.num=n.num+1;
        //if(n.num>j)
        //{
        //    printf("已到%d\n", j);
        //    j=j+1000000;
        //}
        pthread_mutex_unlock(&n.mutex);
    }
    ret=pthread_join(pthread, NULL);
    if(ret!=0)
    {
        printf("pthread_join failed %d\n", ret);
        return -1;
    }
    gettimeofday(&end, NULL);
    printf("n.num=%d, time=%ld\n", n.num, end.tv_usec-begin.tv_usec+(end.tv_sec-begin.tv_sec)*1000000);
}
